#ifndef SSDCAMERA_H
#define SSDCAMERA_H

#include <stdlib.h>
#include <string>
#include <fstream>
#include <vector>
#include <array>
#include <algorithm>
#if __GNUC__ >= 4 && __GNUC_MINOR__ >= 4 || __APPLE__
    #include <atomic>
#else
    #include <cstdatomic>
#endif

#include <QDebug>

#include "alphanum.hpp"

#include "osutils.h"
#include "cameramodel.h"
#include "constants.h"
#include "lvframe.h"

#define TIMEOUT_DURATION 100

class SSDCamera : public CameraModel
{
public:
    SSDCamera(const std::string search_dir, int frWidth = 640, int frHeight = 480, int dataHeight = 480);
    virtual ~SSDCamera();

    virtual bool start();
    virtual void setDir(const char* dirname);

    virtual uint16_t *getFrame();

private:
    std::string getFname();
    void readFile();

    std::atomic<bool> frame_valid;
    std::ifstream dev_p;
    std::string ifname;
    std::string data_dir;
    std::streampos bufsize;
    const uint16_t nFrames;
    std::atomic<uint32_t> framesize;
    const uint32_t headsize;
    size_t image_no;
    std::vector<std::string> xio_files;
    std::vector< std::vector<uint16_t> > frame_buf;
    std::vector<uint16_t> dummy;
    std::atomic<uint16_t> curIndex;
};

#endif // SSDCAMERA_H
