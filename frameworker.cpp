#include "frameworker.h"

class LVFrameBuffer
{
public:
    LVFrameBuffer(const uint16_t num_frames, const uint16_t width, const uint16_t height)
    {
        for (int f = 0; f < num_frames; ++f) {
            LVFrame* pFrame = new LVFrame(width, height);
            frame_vec.push_back(pFrame);
        }
        fbIndex = 0;
    }
    ~LVFrameBuffer()
    {
        frame_vec.clear();
        std::vector<LVFrame*>(frame_vec).swap(frame_vec);
    }
    void reset(const uint16_t num_frames, const uint16_t width, const uint16_t height)
    {
        frame_vec.clear();
        std::vector<LVFrame*>(frame_vec).swap(frame_vec);
        for (int f = 0; f < num_frames; ++f) {
            LVFrame* pFrame = new LVFrame(width, height);
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
    inline uint16_t incIndex() { return fbIndex + 1 >= frame_vec.size() ? 0 : fbIndex + 1; }
};

FrameWorker::FrameWorker(FrameThread *worker, QObject *parent) : QObject(parent), thread(worker)
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
        isRunning = true;    // now set up to enter the event loop
    }
    lvframe_buffer = new LVFrameBuffer(cpu_frame_buffer_size, frWidth, frHeight);
}

FrameWorker::~FrameWorker()
{
    delete Camera;
    delete lvframe_buffer;
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

void FrameWorker::captureFrames()
{
    qDebug("About to start capturing frames");
    size_t dsf_size = sizeof(lvframe_buffer->current()->dsf_data)/sizeof(*lvframe_buffer->current()->dsf_data);
    high_resolution_clock::time_point beg, end;
    uint32_t duration;
    while (isRunning) {
        beg = high_resolution_clock::now();
        lvframe_buffer->current()->raw_data = Camera->getFrame();
        end = high_resolution_clock::now();


        duration = duration_cast<microseconds>(end - beg).count();
        if (duration < 10) {
            thread->sleep(10 - duration);
        }

        for (unsigned int n = 0; n < dsf_size; n++) {
            lvframe_buffer->current()->dsf_data[n] = 50;
        }
    }
}

void FrameWorker::resetDir(const char *dirname)
{
    if (cam_type == ITB) {
        Camera->setDir(dirname);
    }
}

uint16_t* FrameWorker::getFrame()
{
    return lvframe_buffer->current()->raw_data;
}

uint16_t* FrameWorker::getDSFrame()
{
    return lvframe_buffer->current()->dsf_data;
}
