#ifndef CAMERAMODEL_H
#define CAMERAMODEL_H

#include <stdint.h>
#include <atomic>

#include <QObject>
#include <QDebug>

#include "image_type.h"

class CameraModel : public QObject
{
    Q_OBJECT

public:
    CameraModel(QObject *parent = nullptr) : QObject(parent) { running.store(false); }
    virtual ~CameraModel() { running.store(false); }


    virtual bool start() = 0;
    virtual uint16_t *getFrame() = 0;

    virtual void setDir(const char *filename) { Q_UNUSED(filename); }

    virtual bool isRunning() { return running.load(); }

    unsigned int getFrameWidth() const { return frame_width; }
    unsigned int getFrameHeight() const { return frame_height; }
    unsigned int getDataHeight() const { return data_height; }
    virtual char* getCameraName() const { return camera_name; }
    camera_t getCameraType() const { return camera_type; }
    source_t getSourceType() const { return source_type; }

signals:
    void timeout();

protected:
    unsigned int frame_width;
    unsigned int frame_height;
    unsigned int data_height;
    char *camera_name;
    camera_t camera_type;
    source_t source_type;

    std::atomic<bool> running;
};

#endif // CAMERAMODEL_H
