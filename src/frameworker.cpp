#include "frameworker.h"

class LVFrameBuffer
{
public:
    LVFrameBuffer(const unsigned int num_frames, const unsigned int frame_width, const unsigned int frame_height)
        : lastIndex(0), fbIndex(0),  dsfIndex(0), stdIndex(0)
    {
        for (unsigned int f = 0; f < num_frames; ++f) {
            auto pFrame = new LVFrame(frame_width, frame_height);
            frame_vec.push_back(pFrame);
        }
    }
    ~LVFrameBuffer()
    {
        for(auto &elem : frame_vec) {
            delete elem;
            elem = nullptr;
        }
        frame_vec.clear();
        std::vector<LVFrame*>(frame_vec).swap(frame_vec);
        Q_ASSERT(frame_vec.capacity() == 0);
    }
    void reset(const unsigned int num_frames, const unsigned int frame_width, const unsigned int frame_height)
    {
        for(auto &elem : frame_vec) {
            delete elem;
            elem = nullptr;
        }
        frame_vec.clear();
        std::vector<LVFrame*>(frame_vec).swap(frame_vec);
        for (unsigned int f = 0; f < num_frames; ++f) {
            auto pFrame = new LVFrame(frame_width, frame_height);
            frame_vec.push_back(pFrame);
        }
        fbIndex.store(0, std::memory_order_release);
    }

    size_t size() const { return frame_vec.size(); }

    LVFrame* frame(uint16_t i) { return frame_vec.at(i); }
    LVFrame* current() { return frame_vec.at(uint32_t(fbIndex.load())); }
    LVFrame* recent() { return frame_vec.at(uint32_t(lastIndex.load())); }
    LVFrame* lastDSF() { return frame_vec.at(uint32_t(dsfIndex.load())); }
    LVFrame* lastSTD() { return frame_vec.at(uint32_t(stdIndex.load())); }

    std::atomic<int> lastIndex;
    std::atomic<int> fbIndex;
    std::atomic<int> dsfIndex;
    std::atomic<int> stdIndex;

public slots:
    inline void incIndex()
    {
        lastIndex.store(fbIndex, std::memory_order_release);
        if (++fbIndex == static_cast<int>(frame_vec.size())) {
            fbIndex.store(0, std::memory_order_release);
        }
    }
    inline void setDSF(int f_num) { dsfIndex.store(f_num, std::memory_order_release); }
    inline void setSTD(int f_num) { stdIndex.store(f_num, std::memory_order_release); }
private:
    std::vector<LVFrame*> frame_vec;
};

FrameWorker::FrameWorker(QSettings *settings_arg, QThread *worker, QObject *parent)
    : QObject(parent), settings(settings_arg),
      thread(worker), plotMode(LV::pmRAW), saving(false),
      count(0), count_prev(0), frame_period_ms(25.0)
{
    pixRemap = settings->value(QString("pix_remap"), false).toBool();
    is16bit = settings->value(QString("remap16"), false).toBool();
    Camera = nullptr;

    switch(static_cast<source_t>(settings->value(QString("cam_model")).toInt())) {
    case SSD:
        Camera = new SSDCamera(settings->value(QString("ssd_width"), 640).toUInt(),
                               settings->value(QString("ssd_height"), 480).toUInt(),
                               settings->value(QString("ssd_height"), 480).toUInt());
        break;
    case DEBUG:
        Camera = new ENVICamera();
        break;
    case CAMERA_LINK:
#if !(__APPLE__ || __MACH__)
        Camera = new CLCamera();
        break;
#else
        qFatal("Unable to use Camera Link interface on MacOS systems!");
#endif
    }

    bool cam_started = Camera->start();

    if (!cam_started) {
        emit error(QString("Unable to start camera stream! This is fatal."));
        frWidth = 0;
        frHeight = 0;
        cam_type = DEFAULT;
        isRunning = false;    // want to make sure that we don't enter the event loop
    } else {
        frWidth = Camera->getFrameWidth();
        frHeight = Camera->getFrameHeight();
        dataHeight = Camera->getDataHeight();
        cam_type = Camera->getCameraType();

        if (frWidth == 0 || frHeight == 0) {
            isRunning = false;
            qFatal("Frame width and height can not be zero, please initialize camera.");
        } else {
            connect(Camera, &CameraModel::timeout, this, &FrameWorker::reportTimeout);
            isRunning = true;    // now set up to enter the event loop
        }
    }

    frSize = frWidth * dataHeight;
    lvframe_buffer = new LVFrameBuffer(CPU_FRAME_BUFFER_SIZE, frWidth, dataHeight);
    TwosFilter = new TwosComplimentFilter(frSize);
    DSFilter = new DarkSubFilter(frSize);
    stddev_N = MAX_N; // arbitrary starting point
    STDFilter = new StdDevFilter(frWidth, dataHeight, stddev_N);
    MEFilter = new MeanFilter(int(frWidth), int(dataHeight));
    if (!STDFilter->start()) {
        qWarning("Unable to start OpenCL kernel.");
        qWarning("Standard Deviation and Histogram computation will be disabled.");
    }

    centerVal = QPointF(-1.0, -1.0);
    topLeft = QPointF(0, 0);
    bottomRight = QPointF(frWidth, frHeight);

    connect(this, &FrameWorker::doneSaving, this, [&]()
    {
        if (!SaveQueue.empty()) {
            const save_req_t &req = SaveQueue.front();
            SaveQueue.pop();
            QtConcurrent::run(this, &FrameWorker::saveFrames, req);
        } else {
            saving = false;
        }
    });

}

FrameWorker::~FrameWorker()
{
    isRunning = false;
    delete Camera;
    delete lvframe_buffer;
    delete DSFilter;
    delete MEFilter;
    delete STDFilter;
}

void FrameWorker::stop()
{
    qDebug("Stopping frame acquistion.");
    isRunning = false;
    emit finished();
}

bool FrameWorker::running()
{
    return isRunning;
}

void FrameWorker::setPlotMode(LV::PlotMode pm)
{
    plotMode = pm;
}

void FrameWorker::reportTimeout()
{
    emit updateFPS(-1.0);
    isTimeout = true;
}

void FrameWorker::captureFrames()
{
    qDebug("About to start capturing frames");
    high_resolution_clock::time_point beg, end;
    high_resolution_clock::time_point last_frame;
    int64_t duration;
    double this_frame_duration;

    auto fpsclock = new QTimer(this);
    connect(fpsclock, &QTimer::timeout, this, &FrameWorker::reportFPS);
    fpsclock->start(1000);
    ticklist.fill(0);

    while (isRunning) {
        beg = high_resolution_clock::now();
        lvframe_buffer->current()->raw_data = Camera->getFrame();
        end = high_resolution_clock::now();
        if (Camera->isRunning() && pixRemap) {
            TwosFilter->apply_filter(lvframe_buffer->current()->raw_data, is16bit);
        }


        duration = duration_cast<milliseconds>(end - beg).count();
        this_frame_duration = duration_cast<microseconds>(end - last_frame).count();
        last_frame = end;
        ticksum -= ticklist.at(size_t(tickindex));
        ticksum += int(this_frame_duration);
        ticklist[size_t(tickindex)] = int(this_frame_duration);
        if (++tickindex == MAXSAMPLES) {
            tickindex = 0;
        }

        lvframe_buffer->incIndex();

        count++;
        if (duration < frame_period_ms && cam_type == SSD_XIO) {
            delay(int64_t(frame_period_ms) - duration);
        } else {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
    }
}

void FrameWorker::captureDSFrames()
{
    int64_t count_framestart;
    uint16_t store_point;
    int64_t last_complete = 1;

    while (isRunning) {
        count_framestart = int64_t(count.load()) - 1;
        if (last_complete < count_framestart) {
            store_point = count_framestart % CPU_FRAME_BUFFER_SIZE;
            DSFilter->dsf_callback(lvframe_buffer->frame(store_point)->raw_data, lvframe_buffer->frame(store_point)->dsf_data);
            MEFilter->compute_mean(lvframe_buffer->frame(store_point), topLeft,
                                   bottomRight, plotMode, Camera->isRunning());
            lvframe_buffer->setDSF(store_point);
            last_complete = count_framestart;
        } else {
            usleep(FRAME_DISPLAY_PERIOD_MSECS * 1000);
        }
    }
}

void FrameWorker::captureSDFrames()
{
    int64_t count_framestart;
    uint16_t store_point;
    int64_t last_complete = 0;

    while (isRunning) {
        count_framestart = int64_t(count.load()) - 1;
        if (last_complete < count_framestart && STDFilter->isReadyRead()) {
            store_point = count_framestart % CPU_FRAME_BUFFER_SIZE;
            STDFilter->compute_stddev(lvframe_buffer->frame(store_point), stddev_N);
            // Move the read point in the buffer only if the data is "valid"
            if (STDFilter->isReadyDisplay()) {
                lvframe_buffer->setSTD(store_point);
                compute_snr(lvframe_buffer->frame(store_point));
            }
            last_complete = count_framestart;
        } else {
            usleep(useconds_t(frame_period_ms) * 1000);
        }
    }
}

void FrameWorker::saveFrames(save_req_t req)
{
    emit startSaving();
    saving = true;
    int64_t next_frame = count.load();
    int64_t new_count = 0;
    std::vector<uint16_t> p_frame;
    std::string hdr_fname;
    save_count.store(0);

    std::vector<float> frame_accum;
    if (req.nAvgs > 1) {
        frame_accum.resize(frSize);
        std::fill(frame_accum.begin(), frame_accum.end(), 0.0);
    }

    if (req.file_name.find_last_of('.') != std::string::npos) {
        hdr_fname = req.file_name.substr(0, req.file_name.find_last_of('.') + 1) + "hdr";
    } else {
        hdr_fname = req.file_name + ".hdr";
    }

    switch(req.bit_org) {
    case fwBIL:
        p_getSaveFrame = &FrameWorker::getBILSaveFrame;
        break;
    case fwBIP:
        p_getSaveFrame = &FrameWorker::getBIPSaveFrame;
        break;
    case fwBSQ:
        p_getSaveFrame = &FrameWorker::getBILSaveFrame; // BSQ conversion is done at the end.
    }

    std::ofstream p_file;
    p_file.open(req.file_name, std::ofstream::binary);
    while (save_count.load() < req.nFrames) {
        new_count = count.load();
        for (int f = 0; f < new_count - next_frame; f++) {
            frame_fifo.push(lvframe_buffer->frame((next_frame + f) % CPU_FRAME_BUFFER_SIZE)->raw_data);
        }
        next_frame = new_count;
        if (!frame_fifo.empty()) {
            p_frame = (this->*p_getSaveFrame)(); // frame_fifo.front();
            if (req.nAvgs <= 1) {
                p_file.write(reinterpret_cast<char*>(p_frame.data()),
                             frSize * sizeof(uint16_t));
            } else {
                if (save_count % req.nAvgs == 0) {
                    for (size_t p = 0; p < frSize; p++) {
                        frame_accum[p] /= static_cast<float>(req.nAvgs);
                    }
                    p_file.write(reinterpret_cast<char*>(frame_accum.data()),
                                 frSize * sizeof(float));
                    std::fill(frame_accum.begin(), frame_accum.end(), 0.0);
                }
                for (size_t p = 0; p < frSize; p++) {
                    frame_accum[p] += p_frame[p];
                }
            }

            frame_fifo.pop();
            save_count++;
        }
    }
    p_file.close();

    if (req.bit_org == fwBSQ) {
        convertBSQ(req);
    }

    std::string hdr_text = "ENVI\ndescription = {LIVEVIEW raw export file, " +
            std::to_string(req.nFrames) + " frame mean per acquisition}\n";
    hdr_text += "samples = " + std::to_string(frWidth) + "\n";
    hdr_text += "lines   = " + std::to_string(req.nFrames / req.nAvgs) + "\n";
    hdr_text += "bands   = " + std::to_string(dataHeight) + "\n";
    hdr_text += "header offset = 0\nfile type = ENVI Standard\ndata type = 12\n";
    hdr_text += "interleave = " + std::to_string(req.bit_org) + "\n";
    hdr_text += "sensor type = Unknown\nbyte order = 0\nwavelength units = Unknown\n";

    std::ofstream hdr_out(hdr_fname);
    hdr_out << hdr_text;
    hdr_out.close();
    qDebug() << "Done saving frames!";
    emit doneSaving();
}

void FrameWorker::captureFramesRemote(const save_req_t &new_req)
{
    SaveQueue.push(new_req);
    if (!saving) {
        const save_req_t &req = SaveQueue.front();
        QtConcurrent::run(this, &FrameWorker::saveFrames, req); // no nAvgs for now.
        SaveQueue.pop();
    }
}

void FrameWorker::applyMask(const QString &fileName)
{
    DSFilter->apply_mask_file(fileName);
}

void FrameWorker::collectMask()
{
    if (QFileInfo::exists(mask_file)) {
        int retval = QMessageBox::warning(nullptr, "Confirm Mask Save",
                             QString("The file: %1 already exists. Are you sure you want to overwrite it?").arg(mask_file),
                             QMessageBox::Ok, QMessageBox::Cancel);
        if (retval == QMessageBox::Accepted) {
            DSFilter->start_mask_collection(avgd_frames);
        } else {
            return;
        }
    } else {
        DSFilter->start_mask_collection(avgd_frames);
    }
}

void FrameWorker::stopCollectingMask()
{
    DSFilter->finish_mask_collection();
    if (!mask_file.isEmpty()) {
        DSFilter->save_mask_file(mask_file);
    }
}

void FrameWorker::setMaskSettings(QString mask_name, quint64 avg_frames)
{
    mask_file = std::move(mask_name);
    avgd_frames = avg_frames;
}

void FrameWorker::setStdDevN(int new_N)
{
    if (new_N < 0) {
        new_N = 0;
    } else if (new_N > MAX_N) {
        new_N = MAX_N;
    }
    stddev_N = static_cast<uint32_t>(new_N);
}

void FrameWorker::reportFPS()
{
    if (Camera->isRunning()) {
        isTimeout = false;
        fps = double(MAXSAMPLES) * 1000000.0 / double(ticksum);
        emit updateFPS(fps);
    }
}

void FrameWorker::resetDir(const char *dirname)
{
    if (cam_type == SSD_XIO) {
        Camera->setDir(dirname);
    }
}

std::vector<float> FrameWorker::getFrame()
{
    //Maintains reference to data by using vector for memory management

    std::vector<float> raw_data(frSize);
    for (unsigned int i = 0; i < frSize; i++) {
        raw_data[i] = float(lvframe_buffer->recent()->raw_data[i]);
    }
    return raw_data;
}

std::vector<float> FrameWorker::getDSFrame()
{
    //Maintains reference to data by using vector for memory management
    return std::vector<float>(lvframe_buffer->lastDSF()->dsf_data, lvframe_buffer->lastDSF()->dsf_data + frSize);
}

std::vector<float> FrameWorker::getSDFrame()
{
    //Maintains reference to data by using vector for memory management

    return std::vector<float>(lvframe_buffer->lastSTD()->sdv_data, lvframe_buffer->lastSTD()->sdv_data + frSize);
}

std::vector<float> FrameWorker::getSNRFrame()
{
    //Maintains reference to data by using vector for memory management
    return std::vector<float>(lvframe_buffer->lastSTD()->snr_data, lvframe_buffer->lastSTD()->snr_data + frSize);
}

uint32_t* FrameWorker::getHistData()
{
    return lvframe_buffer->lastSTD()->hist_data;
}

float* FrameWorker::getSpatialMean()
{
    return lvframe_buffer->lastDSF()->spatial_mean;
}

float* FrameWorker::getSpectralMean()
{
    return lvframe_buffer->lastDSF()->spectral_mean;
}

float* FrameWorker::getFrameFFT()
{
    return lvframe_buffer->lastDSF()->frame_fft;
}

std::vector<uint16_t> FrameWorker::getBILSaveFrame()
{
    std::vector<uint16_t> BIL_frame;
    BIL_frame.resize(frSize);
    uint16_t *p_frame = frame_fifo.front();
    // idempotent operation because data is in BIL
    // format by default. Simply convert to vector.
    for (size_t i = 0; i < frSize; i++) {
        BIL_frame[i] = p_frame[i];
    }
    return BIL_frame;
}

std::vector<uint16_t> FrameWorker::getBIPSaveFrame()
{
    std::vector<uint16_t> BIP_frame;
    BIP_frame.resize(frSize);
    uint16_t *p_frame = frame_fifo.front();
    // transpose the image to convert to BIP
    for (size_t i = 0; i < frHeight; i++) {
        for (size_t j = 0; j < frWidth; j++) {
            BIP_frame[i * frHeight + j] = p_frame[j * frWidth + i];
        }
    }
    return BIP_frame;
}

void FrameWorker::convertBSQ(save_req_t req)
{
    auto numSamps = static_cast<int>(frWidth);
    auto numLines = static_cast<int>(std::ceil(req.nFrames / req.nAvgs));
    int linesPerChunk = std::min(numLines, CHUNK_NUMLINES);
    int readLinesChunk = linesPerChunk;
    int rem = numLines % linesPerChunk > 0 ? 1 : 0;
    int numChunks = numLines / linesPerChunk + rem;
    auto numBands = static_cast<int>(frHeight);
    int pixel_size = sizeof(uint16_t);
    std::vector< std::vector<uint16_t> > BandArray;
    std::vector<uint16_t> LineArray;
    BandArray.resize(static_cast<size_t>(numBands));
    for (auto &band : BandArray) {
        band.resize(static_cast<size_t>(numSamps * linesPerChunk));
    }
    LineArray.resize(static_cast<size_t>(numSamps));

    std::ifstream bil_image;
    std::ofstream bsq_image;
    std::string bsq_filename = req.file_name + ".bsq";

    bil_image.open(req.file_name, std::ios::in | std::ios::binary);
    if (!bil_image.is_open()) {
        qDebug().nospace() << "Could not open file " << req.file_name.c_str() << ". Does it exist?";
        bil_image.clear();
        return;
    }
    bsq_image.open(bsq_filename, std::ios::out | std::ios::binary);
    if (!bsq_image.is_open()) {
        qDebug().nospace() << "Could not open file " << bsq_filename.c_str() << ". Is it already open?";
        bsq_image.clear();
        return;
    }
    for (int chunk = 0; chunk < numChunks; chunk++) {
        if (chunk == numChunks - 1 && numLines % linesPerChunk > 0) {
            readLinesChunk = numLines % linesPerChunk;
        }
        for (int line = 0; line < readLinesChunk; line++) {
            for (auto &band : BandArray) {
                bil_image.read(reinterpret_cast<char*>(LineArray.data()), numSamps * pixel_size);
                band.insert(band.begin() + (line * numSamps), LineArray.begin(), LineArray.end());
            }
        }

        bsq_image.seekp(linesPerChunk * chunk * numSamps * pixel_size);
        for (auto &band : BandArray) {
            bsq_image.write(reinterpret_cast<char*>(band.data()), linesPerChunk * numSamps * pixel_size);
            bsq_image.seekp((numLines - linesPerChunk * (chunk + 1)) * numSamps * pixel_size, std::ios_base::cur);
        }
    }
}

void FrameWorker::delay(int64_t msecs)
{
    QTime remTime = QTime::currentTime().addMSecs(int(msecs));
    while(QTime::currentTime() < remTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

void FrameWorker::setCenter(double Xcoord, double Ycoord)
{
    centerVal.setX(Xcoord);
    centerVal.setY(Ycoord);
    emit crosshairChanged(QPointF(Xcoord, Ycoord));
}

QPointF* FrameWorker::getCenter()
{
    return &centerVal;
}

uint32_t FrameWorker::getStdDevN()
{
    return stddev_N;
}

void FrameWorker::compute_snr(LVFrame *new_frame)
{
    for (unsigned int i = 0; i < new_frame->frSize; ++i) {
        if (new_frame->sdv_data[i] > 0) {
            new_frame->snr_data[i] = new_frame->dsf_data[i] / new_frame->sdv_data[i] * 1000;
        } else {
            new_frame->snr_data[i] = 0;
        }
    }
}

void FrameWorker::setFramePeriod(double period) {
    frame_period_ms = period;
}

double FrameWorker::getFramePeriod() {
    return frame_period_ms;
}
