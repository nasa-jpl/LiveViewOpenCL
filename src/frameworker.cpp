#include "frameworker.h"

class LVFrameBuffer
{
public:
    LVFrameBuffer(const unsigned int num_frames, const unsigned int frame_width, const unsigned int frame_height)
        : lastIndex(0), fbIndex(0),  dsfIndex(0), stdIndex(0)
    {
        for (unsigned int f = 0; f < num_frames; ++f) {
            LVFrame *pFrame = new LVFrame(frame_width, frame_height);
            frame_vec.push_back(pFrame);
        }
    }
    ~LVFrameBuffer()
    {
        frame_vec.clear();
        std::vector<LVFrame*>(frame_vec).swap(frame_vec);
        Q_ASSERT(frame_vec.capacity() == 0);
    }
    void reset(const unsigned int num_frames, const unsigned int frame_width, const unsigned int frame_height)
    {
        frame_vec.clear();
        std::vector<LVFrame*>(frame_vec).swap(frame_vec);
        for (unsigned int f = 0; f < num_frames; ++f) {
            LVFrame* pFrame = new LVFrame(frame_width, frame_height);
            frame_vec.push_back(pFrame);
        }
        fbIndex.store(0, std::memory_order_release);
    }

    uint16_t size() const { return frame_vec.size(); }

    LVFrame* frame(uint16_t i) { return frame_vec.at(i); }
    LVFrame* current() { return frame_vec.at(fbIndex.load()); }
    LVFrame* recent() { return frame_vec.at(lastIndex.load()); }
    LVFrame* lastDSF() { return frame_vec.at(dsfIndex.load()); }
    LVFrame* lastSTD() { return frame_vec.at(stdIndex.load()); }

    std::atomic<int> lastIndex;
    std::atomic<int> fbIndex;
    std::atomic<int> dsfIndex;
    std::atomic<int> stdIndex;

public slots:
    inline void incIndex()
    {
        lastIndex.store(fbIndex, std::memory_order_release);
        if (++fbIndex == (int)frame_vec.size()) {
            fbIndex.store(0, std::memory_order_release);
        }
    }
    inline void setDSF(unsigned int f_num) { dsfIndex.store(f_num, std::memory_order_release); }
    inline void setSTD(unsigned int f_num) { stdIndex.store(f_num, std::memory_order_release); }
private:
    std::vector<LVFrame*> frame_vec;
};

FrameWorker::FrameWorker(QThread *worker, QObject *parent)
    : QObject(parent), thread(worker),
      useDSF(false), saving(false),
      count(0), count_prev(0)
{
    Camera = new SSDCamera();
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
        connect(Camera, &CameraModel::timeout, this, &FrameWorker::reportTimeout);
        isRunning = true;    // now set up to enter the event loop
    }

    frSize = frWidth * dataHeight;
    lvframe_buffer = new LVFrameBuffer(CPU_FRAME_BUFFER_SIZE, frWidth, dataHeight);
    DSFilter = new DarkSubFilter(frSize);
    stddev_N = MAX_N;
    STDFilter = new StdDevFilter(frWidth, dataHeight, stddev_N);
    MEFilter = new MeanFilter(frWidth, dataHeight);
    if (!STDFilter->start()) {
        qWarning("Unable to start OpenCL kernel.");
        qWarning("Standard Deviation and Histogram computation will be disabled.");
    }

    centerVal += QPointF(-1.0, -1.0);

    connect(this, &FrameWorker::doneSaving, this, [&]()
    {
        if (!SaveQueue.empty()) {
            const save_req_t &req = SaveQueue.front();
            SaveQueue.pop();
            QtConcurrent::run(this, &FrameWorker::saveFrames, req.file_name, req.nFrames); // no nAvgs for now.
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
    delete STDFilter;
    delete MEFilter;
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

void FrameWorker::setDSF(bool toggled)
{
    useDSF = toggled;
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
    uint32_t duration;

    QTimer *fpsclock = new QTimer(this);
    connect(fpsclock, &QTimer::timeout, this, &FrameWorker::reportFPS);
    fpsclock->start(1000);

    while (isRunning) {
        beg = high_resolution_clock::now();
        lvframe_buffer->current()->raw_data = Camera->getFrame();
        end = high_resolution_clock::now();

        duration = duration_cast<milliseconds>(end - beg).count();
        lvframe_buffer->incIndex();

        count++;
        if (duration < FRAME_PERIOD_MS && cam_type == ITB) {
            delay(FRAME_PERIOD_MS - duration);
        }
    }
}

void FrameWorker::captureDSFrames()
{
    int64_t count_framestart;
    uint16_t store_point;
    int64_t last_complete = 1;

    while (isRunning) {
        count_framestart = count.load() - 1;
        if (last_complete < count_framestart) {
            store_point = count_framestart % CPU_FRAME_BUFFER_SIZE;
            DSFilter->dsf_callback(lvframe_buffer->frame(store_point)->raw_data, lvframe_buffer->frame(store_point)->dsf_data);
            if(Camera->isRunning())
                MEFilter->compute_mean(lvframe_buffer->frame(store_point), QPointF((qreal)0, (qreal)0),
                                       QPointF((qreal)frWidth, (qreal)dataHeight), useDSF);
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
        count_framestart = count.load() - 1;
        if (last_complete < count_framestart && STDFilter->isReadyRead()) {
            store_point = count_framestart % CPU_FRAME_BUFFER_SIZE;
            STDFilter->compute_stddev(lvframe_buffer->frame(store_point), stddev_N);
            // Move the read point in the buffer only if the data is "valid"
            if (STDFilter->isReadyDisplay()) {
                lvframe_buffer->setSTD(store_point);
            }
            last_complete = count_framestart;
        } else {
            usleep(FRAME_PERIOD_MS * 1000);
        }
    }
}

void FrameWorker::saveFrames(std::string frame_fname, uint64_t num_frames)
{
    emit startSaving();
    saving = true;
    unsigned int next_frame = count.load();
    uint16_t* p_frame = nullptr;
    std::string hdr_fname;
    save_count.store(0);

    if (frame_fname.find_last_of(".") != std::string::npos) {
        hdr_fname = frame_fname.substr(0, frame_fname.find_last_of(".") + 1) + "hdr";
    } else {
        hdr_fname = frame_fname + ".hdr";
    }

    std::ofstream p_file;
    p_file.open(frame_fname, std::ofstream::binary);
    while (save_count.load() < num_frames) {
        if (count.load() > next_frame) {
            frame_fifo.push(lvframe_buffer->frame(next_frame % CPU_FRAME_BUFFER_SIZE)->raw_data);
        }
        if (!frame_fifo.empty()) {
            p_frame = frame_fifo.front();
            p_file.write(reinterpret_cast<char*>(p_frame), frWidth * dataHeight * sizeof(uint16_t));
            frame_fifo.pop();
            if (++next_frame == CPU_FRAME_BUFFER_SIZE) { next_frame = 0; }
            save_count++;
        }
    }
    p_file.close();

    std::string hdr_text = "ENVI\ndescription = {LIVEVIEW raw export file, " + std::to_string(num_frames) + " frame mean per acquisition}\n";
    hdr_text += "samples = " + std::to_string(frWidth) + "\n";
    hdr_text += "lines   = " + std::to_string(num_frames) + "\n";
    hdr_text += "bands   = " + std::to_string(dataHeight) + "\n";
    hdr_text += "header offset = 0\nfile type = ENVI Standard\n";
    hdr_text += "data type = 12\n";
    hdr_text += "interleave = bil\nsensor type = Unknown\nbyte order = 0\nwavelength units = Unknown\n";

    std::ofstream hdr_out(hdr_fname);
    hdr_out << hdr_text;
    hdr_out.close();
    qDebug() << "Done saving frames!";
    emit doneSaving();
}

void FrameWorker::captureFramesRemote(const QString &fileName, const quint64 &nFrames, const quint64 &nAvgs)
{
    SaveQueue.push({fileName.toStdString(), nFrames, nAvgs});
    if (!saving) {
        const save_req_t &req = SaveQueue.front();
        QtConcurrent::run(this, &FrameWorker::saveFrames, req.file_name, req.nFrames); // no nAvgs for now.
        SaveQueue.pop();
    }
}

void FrameWorker::reportFPS()
{
    if (Camera->isRunning()) {
        isTimeout = false;
        emit updateFPS((float)(count.load() - count_prev));
    }
    count_prev = count.load();
}

void FrameWorker::resetDir(const char *dirname)
{
    if (cam_type == ITB) {
        Camera->setDir(dirname);
    }
}

float* FrameWorker::getFrame()
{
    std::vector<float> raw_data(frSize);
    for (unsigned int i = 0; i < frSize; i++) {
        raw_data[i] = (float)lvframe_buffer->recent()->raw_data[i];
    }
    return raw_data.data();
}

float* FrameWorker::getDSFrame()
{
    return lvframe_buffer->lastDSF()->dsf_data;
}

float* FrameWorker::getSDFrame()
{
    return lvframe_buffer->lastSTD()->sdv_data;
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

void FrameWorker::delay(int msecs)
{
    QTime remTime = QTime::currentTime().addMSecs(msecs);
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
