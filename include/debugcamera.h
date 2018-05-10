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
    DebugCamera(const std::string fname,
                unsigned int frWidth = 640,
                unsigned int frHeight = 480,
                unsigned int dataHeight = 481,
                QObject *parent = NULL);
    virtual ~DebugCamera();

    virtual bool start();

    virtual uint16_t *getFrame();

private:
    std::ifstream dev_p;
    std::string ifname;
    std::streampos bufsize;
    const unsigned int framesize;
    std::vector< std::vector<uint16_t> > frame_buf;
    unsigned int curIndex;
    unsigned int nFrames;
};

#endif // DEBUGCAMERA_H
