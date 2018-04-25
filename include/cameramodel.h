#ifndef CAMERAMODEL_H
#define CAMERAMODEL_H

#include <stdint.h>
#include <QtGlobal>
#include "image_type.h"

class CameraModel
{
public:
    CameraModel() {}
    virtual ~CameraModel() {}

    virtual bool start() = 0;
    virtual uint16_t* getFrame() = 0;

    virtual void setDir(const char* filename) = 0;

    virtual bool isRunning() { return true; }

    uint16_t getFrameWidth() const { return frame_width; }
    uint16_t getFrameHeight() const { return frame_height; }
    uint16_t getDataHeight() const { return data_height; }
    camera_t getCameraType() const { return camera_type; }

protected:
    uint16_t frame_width;
    uint16_t frame_height;
    uint16_t data_height;
    camera_t camera_type;
};

#endif // CAMERAMODEL_H
