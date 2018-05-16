#include "frameworker.h"

class LVFrameBuffer
{
public:
    LVFrameBuffer(const unsigned int num_frames, const unsigned int frame_size)
    {
        for (unsigned int f = 0; f < num_frames; ++f) {
            LVFrame* pFrame = new LVFrame(frame_size);
            frame_vec.push_back(pFrame);
        }
        fbIndex = 0;
    }
    ~LVFrameBuffer()
    {
        frame_vec.clear();
        std::vector<LVFrame*>(frame_vec).swap(frame_vec);
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

    void setNextFrame() { fbIndex = incIndex(); }

private:
    std::vector<LVFrame*> frame_vec;
    uint16_t fbIndex;
    inline uint16_t incIndex() { return (size_t)(fbIndex + 1) >= frame_vec.size() ? 0 : fbIndex + 1; }
};

FrameWorker::FrameWorker(FrameThread *worker, QObject *parent) : QObject(parent), thread(worker)
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
        connect(Camera, SIGNAL(timeout()), this, SLOT(timeout()));
        isRunning = true;    // now set up to enter the event loop
    }
    frSize = frWidth * dataHeight;
    lvframe_buffer = new LVFrameBuffer(cpu_frame_buffer_size, frSize);
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

void FrameWorker::timeout()
{
    emit updateFPS(-1.0);
}

void FrameWorker::captureFrames()
{
    qDebug("About to start capturing frames");
    // high_resolution_clock::time_point beg, end;
    // uint32_t duration;
    count = 0;
    clock.start();
    while (isRunning) {
        // beg = high_resolution_clock::now();
        lvframe_buffer->current()->raw_data = Camera->getFrame();
        // end = high_resolution_clock::now();

        //duration = duration_cast<microseconds>(end - beg).count();
        /*if (duration < 10) {
            thread->sleep(10 - duration);
        }*/
        DSFilter->dsf_callback(lvframe_buffer->current()->raw_data, lvframe_buffer->current()->dsf_data);
        STDFilter->compute_stddev(lvframe_buffer->current(), stddev_N);

        count++;
        if (count % 100 == 0) {
            if (Camera->isRunning()) {
                emit updateFPS(100.0 / clock.restart() * 1000.0);
            }
        }
    }
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
        raw_data[i] = (float)lvframe_buffer->current()->raw_data[i];
    }
    return raw_data.data();
}

float* FrameWorker::getDSFrame()
{
    return lvframe_buffer->current()->dsf_data;
}

float* FrameWorker::getSDFrame()
{
    return lvframe_buffer->current()->sdv_data;
}

uint32_t* FrameWorker::getHistData()
{
    return lvframe_buffer->current()->hist_data;
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
