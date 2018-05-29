#include "frameworker.h"

class LVFrameBuffer
{
public:
    LVFrameBuffer(const unsigned int num_frames, const unsigned int frame_size)
        : lastIndex(0), fbIndex(0),  dsfIndex(0), stdIndex(0)
    {
        for (unsigned int f = 0; f < num_frames; ++f) {
            LVFrame *pFrame = new LVFrame(frame_size);
            frame_vec.push_back(pFrame);
        }
    }
    ~LVFrameBuffer()
    {
        frame_vec.clear();
        std::vector<LVFrame*>(frame_vec).swap(frame_vec);
        Q_ASSERT(frame_vec.capacity() == 0);
    }
    void reset(const unsigned int num_frames, const unsigned int frame_size)
    {
        frame_vec.clear();
        std::vector<LVFrame*>(frame_vec).swap(frame_vec);
        for (unsigned int f = 0; f < num_frames; ++f) {
            LVFrame* pFrame = new LVFrame(frame_size);
            frame_vec.push_back(pFrame);
        }
        fbIndex = 0;
    }

    uint16_t size() const { return frame_vec.size(); }

    LVFrame* frame(uint16_t i) { return frame_vec.at(i); }
    LVFrame* current() { return frame_vec.at(fbIndex); }
    LVFrame* recent() { return frame_vec.at(lastIndex); }
    LVFrame* lastDSF() { return frame_vec.at(dsfIndex); }
    LVFrame* lastSTD() { return frame_vec.at(stdIndex); }

    volatile int lastIndex;
    std::atomic<int> fbIndex;
    std::atomic<int> dsfIndex;
    std::atomic<int> stdIndex;

public slots:
    inline void incIndex() { lastIndex = fbIndex; if (++fbIndex == (int)frame_vec.size()) { fbIndex = 0; } }
    inline void incDSF() { if (++dsfIndex == (int)frame_vec.size()) { dsfIndex = 0; } }
    inline void incSTD() { if (++stdIndex == (int)frame_vec.size()) { stdIndex = 0; } }

private:
    std::vector<LVFrame*> frame_vec;
    uint16_t fbIndex;
    inline uint16_t incIndex() { return (size_t)(fbIndex + 1) >= frame_vec.size() ? 0 : fbIndex + 1; }
};

FrameWorker::FrameWorker(QThread *worker, QObject *parent)
    : QObject(parent), thread(worker),
      count(0), count_prev(0),
      save_framenum(0), save_count(0), save_num_avgs(1)
{
    // const std::string search_dir = "/";
    Camera = new CLCamera();
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
    lvframe_buffer = new LVFrameBuffer(CPU_FRAME_BUFFER_SIZE, frSize);
    DSFilter = new DarkSubFilter(frSize);
    stddev_N = MAX_N;
    STDFilter = new StdDevFilter(frWidth, dataHeight, stddev_N);
    if (!STDFilter->start()) {
        qWarning("Unable to start OpenCL kernel.");
        qWarning("Standard Deviation and Histogram computation will be disabled.");
    }

    saveframe_list.clear();

    centerVal += QPointF(-1.0, -1.0);

}

FrameWorker::~FrameWorker()
{
    delete Camera;
    delete lvframe_buffer;
    delete DSFilter;
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

void FrameWorker::reportTimeout()
{
    emit updateFPS(-1.0);
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
        if (duration < FRAME_PERIOD_MS) {
            delay(FRAME_PERIOD_MS - duration);
        }

        lvframe_buffer->incIndex();

        count++;
    }
}

void FrameWorker::captureDSFrames()
{
    int last_complete = 0;
    while (isRunning) {
        if (last_complete != lvframe_buffer->lastIndex) {
            DSFilter->dsf_callback(lvframe_buffer->recent()->raw_data, lvframe_buffer->recent()->dsf_data);
            lvframe_buffer->incDSF();
            last_complete = lvframe_buffer->lastIndex;
        }
    }
}

void FrameWorker::captureSDFrames()
{
    // high_resolution_clock::time_point beg, end;
    // uint32_t duration;
    int last_complete = 0;
    while (isRunning) {
        if (last_complete != lvframe_buffer->lastIndex) {
            // beg = high_resolution_clock::now();
            STDFilter->compute_stddev(lvframe_buffer->recent(), stddev_N);
            // end = high_resolution_clock::now();
            // duration = duration_cast<microseconds>(end - beg).count();
            lvframe_buffer->incSTD();
            last_complete = lvframe_buffer->lastIndex;
        }
    }
}

void FrameWorker::saveFrames(std::string fname_out, unsigned int num_avgs, unsigned int num_frames)
{
    save_framenum.store(0, std::memory_order_seq_cst);
    save_count.store(0, std::memory_order_seq_cst);
    save_framenum.store(num_frames, std::memory_order_seq_cst);
    save_count.store(0, std::memory_order_seq_cst);

    std::string hdr_fname = fname_out.substr(0, fname_out.size() - 3) + "hdr";
    FILE *file_out = fopen(fname_out.c_str(), "wb");

    int sv_count = 0;

    while (save_framenum != 0 || !saveframe_list.empty()) {
        if (!saveframe_list.empty()) {
            if (num_avgs == 1) {
                uint16_t *data = saveframe_list.back();
                saveframe_list.pop_back();
                fwrite(data, sizeof(uint16_t), frWidth * dataHeight, file_out);
                delete[] data;
                sv_count++;
                if (sv_count == 1) {
                    save_count.store(1, std::memory_order_seq_cst);
                } else {
                    save_count++;
                }
            } else if (saveframe_list.size() >= num_avgs && num_avgs != 1) {
                float *data = new float[frWidth * dataHeight];
                for (unsigned int i2 = 0; i2 < num_avgs; i2++) {
                    uint16_t *data2 = saveframe_list.back();
                    saveframe_list.pop_back();
                    if (i2 == 0) {
                        for (unsigned int i = 0; i < frWidth * dataHeight; i++) {
                            data[i] = (float)data2[i];
                        }
                    } else if (i2 == num_avgs - 1) {
                        for (unsigned int i = 0; i < frWidth * dataHeight; i++) {
                            data[i] = (data[i] + (float)data2[i]) / num_avgs;
                        }
                    } else {
                        for (unsigned int i = 0; i < frWidth * dataHeight; i++) {
                            data[i] += (float)data2[i];
                        }
                    }
                    delete[] data2;
                }
                fwrite(data, sizeof(float), frWidth * dataHeight, file_out);
                delete[] data;
                sv_count++;
                if (sv_count == 1) {
                    save_count.store(1, std::memory_order_seq_cst);
                } else {
                    save_count++;
                }
            } else if (save_framenum == 0 && saveframe_list.size() < num_avgs) {
                saveframe_list.erase(saveframe_list.begin(), saveframe_list.end());
            } else {
                usleep(250);
            }
        } else {
            usleep(250);
        }
    }
    fclose(file_out);

    std::string hdr_text = "ENVI\ndescription = {LIVEVIEW raw export file, " + std::to_string(num_avgs) + " frame mean per acquisition}\n";
    hdr_text += "samples = " + std::to_string(frWidth) + "\n";
    hdr_text += "lines   = " + std::to_string(sv_count) + "\n";
    hdr_text += "bands   = " + std::to_string(dataHeight) + "\n";
    hdr_text += "header offset = 0\nfile type = ENVI Standard\n";
    if (num_avgs != 1) {
        hdr_text += "data type = 4\n";
    } else {
        hdr_text += "data type = 12\n";
    }
    hdr_text += "interleave = bil\nsensor type = Unknown\nbyte order = 0\nwavelength units = Unknown\n";

    std::ofstream hdr_out(hdr_fname);
    hdr_out << hdr_text;
    hdr_out.close();
    save_count.store(0, std::memory_order_seq_cst);
    qDebug() << "Done saving frames!";
    emit doneSaving();
}

void FrameWorker::reportFPS()
{
    if (Camera->isRunning()) {
        emit updateFPS((float)(count - count_prev));
    }
    count_prev = count;
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
