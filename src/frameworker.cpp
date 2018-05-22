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
};

FrameWorker::FrameWorker(QThread *worker, QObject *parent)
    : QObject(parent), thread(worker),
      count(0), count_prev(0)
{
    const std::string search_dir = "/";
    Camera = new SSDCamera(search_dir);
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
    // QTime clock;
    // clock.start();

    QTimer *fpsclock = new QTimer(this);
    connect(fpsclock, SIGNAL(timeout()), this, SLOT(reportFPS()));
    fpsclock->start(1000);

    /* FutureManager *DSman = new FutureManager();
    QFuture<void> DSCompute;
    connect(DSman, &FutureManager::incIndex,
            lvframe_buffer, &LVFrameBuffer::incDSF);
    FutureManager *STDman = new FutureManager();
    QFuture<void> STDCompute;
    connect(STDman, &FutureManager::incIndex,
            lvframe_buffer, &LVFrameBuffer::incSTD);
    */

    while (isRunning) {
        beg = high_resolution_clock::now();
        lvframe_buffer->current()->raw_data = Camera->getFrame();
        end = high_resolution_clock::now();

        duration = duration_cast<milliseconds>(end - beg).count();
        if (duration < 100) {
            delay(100 - duration);
        } else {
            qDebug() << duration;
        }

        // DSCompute = QtConcurrent::run(DSFilter, &DarkSubFilter::dsf_callback, lvframe_buffer->current()->raw_data, lvframe_buffer->current()->dsf_data);
        // DSman->addFuture(DSCompute);
        // STDCompute = QtConcurrent::run(STDFilter, &StdDevFilter::compute_stddev, lvframe_buffer->current(), stddev_N);
        // STDman->addFuture(STDCompute);
        // DSFilter->dsf_callback(lvframe_buffer->current()->raw_data, lvframe_buffer->current()->dsf_data);
        // STDFilter->compute_stddev(lvframe_buffer->current(), stddev_N);
        lvframe_buffer->incIndex();

        count++;
        /* if (count % 100 == 0) {
            if (Camera->isRunning()) {
                emit updateFPS(100.0 / clock.restart() * 1000.0);
            }
        } */
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
