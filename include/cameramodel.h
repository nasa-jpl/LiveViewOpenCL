#ifndef CAMERAMODEL_H
#define CAMERAMODEL_H

#include <stdint.h>
#include <QObject>
#include <QDebug>
#include "image_type.h"

#if __GNUC__ >= 4 && __GNUC_MINOR__ >= 4 || __APPLE__
    #include <atomic>
#else
    #include <cstdatomic>
#endif

class CameraModel : public QObject
{
    Q_OBJECT

public:
    CameraModel(QObject *_parent = NULL) : QObject(_parent), running(false) {}
    virtual ~CameraModel() {}


    virtual bool start() = 0;
    virtual uint16_t* getFrame() = 0;

    virtual void setDir(const char* filename) = 0;

    virtual bool isRunning() { return running; }

    uint16_t getFrameWidth() const { return frame_width; }
    uint16_t getFrameHeight() const { return frame_height; }
    uint16_t getDataHeight() const { return data_height; }
    camera_t getCameraType() const { return camera_type; }

signals:
    void timeout();

protected:
    uint16_t frame_width;
    uint16_t frame_height;
    uint16_t data_height;
    camera_t camera_type;

    std::atomic<bool> running;
};

#endif // CAMERAMODEL_H
