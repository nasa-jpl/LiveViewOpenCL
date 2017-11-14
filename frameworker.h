#ifndef FRAMEWORKER_H
#define FRAMEWORKER_H

#include <stdint.h>
#include <vector>

#include <QObject>

#include "image_type.h"
#include "lvframe.h"
#include "cameramodel.h"
#include "debugcamera.h"
#include "constants.h"

class LVFrameBuffer;

class FrameWorker : public QObject
{
    Q_OBJECT

public:
    explicit FrameWorker(QObject *parent = nullptr);
    virtual ~FrameWorker();
    void stop();
    bool running();

    uint16_t *getFrame();

    uint16_t getFrameWidth() const { return frWidth; }
    uint16_t getFrameHeight() const { return frHeight; }
    uint16_t getDataHeight() const { return dataHeight; }
    camera_t getCameraType() const { return cam_type; }

signals:
    void finished();
    void error(const QString &error);

public slots:
    void captureFrames();

private:
    const uint16_t cpu_frame_buffer_size = CPU_FRAME_BUFFER_SIZE;
    LVFrameBuffer* lvframe_buffer;
    CameraModel* Camera;
    bool pixRemap;
    bool isRunning;
    uint16_t frWidth, frHeight, dataHeight;
    camera_t cam_type;

};

#endif // FRAMEWORKER_H
