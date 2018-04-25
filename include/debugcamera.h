#ifndef DEBUGCAMERA_H
#define DEBUGCAMERA_H

#include <string>
#include <fstream>
#include <vector>
#include <array>

#include <QDebug>

#include "cameramodel.h"
#include "constants.h"
#include "lvframe.h"

class DebugCamera : public CameraModel
{
    Q_OBJECT

public:
    DebugCamera(const std::string fname = "./file1.raw", int frWidth = 640, int frHeight = 480, int dataHeight = 481, QObject *_parent = NULL);
    virtual ~DebugCamera();

    virtual bool start();

    virtual uint16_t *getFrame();

private:
    QObject *parent;
    std::ifstream dev_p;
    std::string ifname;
    std::streampos bufsize;
    const uint32_t framesize;
    std::vector<std::array<uint16_t, MAX_SIZE>> frame_buf;
    uint16_t curIndex;
    uint16_t nFrames;
};

#endif // DEBUGCAMERA_H
