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

    uint16_t size() const { return frame_vec.size(); }

    LVFrame* frame(uint16_t i) { return frame_vec.at(i); }
    LVFrame* current() { return frame_vec.at(fbIndex); }

    void setNextFrame() { fbIndex = incIndex(); }

private:
    std::vector<LVFrame*> frame_vec;
    uint16_t fbIndex;
    inline uint16_t incIndex() { return fbIndex + 1 >= frame_vec.size() ? 0 : fbIndex + 1; }
};

FrameWorker::FrameWorker(QObject *parent) : QObject(parent)
{
    Camera = new DebugCamera();
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
    while (isRunning) {
        lvframe_buffer->current()->raw_data = Camera->getFrame();

    }
}

uint16_t* FrameWorker::getFrame()
{
    return lvframe_buffer->current()->raw_data;
}
