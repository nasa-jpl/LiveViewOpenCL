#ifndef FRAMEWORKER_H
#define FRAMEWORKER_H

#include <stdint.h>
#include <vector>
#include <chrono>

#include <QObject>
#include <QTime>

#include "image_type.h"
#include "lvframe.h"
#include "framethread.h"
#include "cameramodel.h"
#include "debugcamera.h"
#include "ssdcamera.h"
#include "darksubfilter.h"
#include "constants.h"

class LVFrameBuffer;

using namespace std::chrono;

class FrameWorker : public QObject
{
    Q_OBJECT

public:
    explicit FrameWorker(FrameThread *worker, QObject *parent = NULL);
    virtual ~FrameWorker();
    void stop();
    bool running();

    float *getFrame();

    DarkSubFilter* DSFilter;
    float *getDSFrame();

    uint16_t getFrameWidth() const { return frWidth; }
    uint16_t getFrameHeight() const { return frHeight; }
    uint16_t getDataHeight() const { return dataHeight; }
    camera_t getCameraType() const { return cam_type; }

signals:
    void finished();
    void error(const QString &error);
    void updateFPS(float fps);

public slots:
    void timeout();
    void captureFrames();
    void resetDir(const char *dirname);

private:
    FrameThread* thread;
    const uint16_t cpu_frame_buffer_size = CPU_FRAME_BUFFER_SIZE;
    LVFrameBuffer* lvframe_buffer;
    CameraModel* Camera;

    bool pixRemap;
    bool isRunning;
    std::atomic<bool> isTimeout; // confusingly, isRunning is the acqusition state, isTimeout just says whether frames are currently coming across the bus.
    QTime clock;
    unsigned int count;
    unsigned int frWidth, frHeight, dataHeight, frSize;
    camera_t cam_type;

};

#endif // FRAMEWORKER_H
