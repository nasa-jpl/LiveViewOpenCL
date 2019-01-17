#ifndef SSDCAMERA_H
#define SSDCAMERA_H

#include <stdlib.h>
#include <cstring>
#include <fstream>
#include <vector>
#include <array>
#include <algorithm>
#include <chrono>

#include <QDebug>
#include <QDir>

#include "alphanum.hpp"

#include "osutils.h"
#include "cameramodel.h"
#include "constants.h"
#include "lvframe.h"

#define TIMEOUT_DURATION 100

using namespace std::chrono;

class SSDCamera : public CameraModel
{
    Q_OBJECT

public:
    SSDCamera(unsigned int frWidth = 640,
              unsigned int frHeight = 480,
              unsigned int dataHeight = 480,
              QObject *parent = nullptr);
    ~SSDCamera();

    virtual bool start();
    virtual void setDir(const char *dirname);

    virtual uint16_t* getFrame();

private:
    std::string getFname();
    void readFile();

    bool is_reading; // Flag that is true while reading from a directory
    std::ifstream dev_p;
    std::string ifname;
    std::string data_dir;
    std::streampos bufsize;
    const unsigned int nFrames;
    unsigned int framesize;
    const unsigned int headsize;

    size_t image_no;
    std::vector<std::string> xio_files;
    std::vector< std::vector<uint16_t> > frame_buf;
    std::vector<unsigned char> header;
    std::vector<uint16_t> dummy;
    std::vector<uint16_t> temp_frame;

    QFuture<void> readLoopFuture;
    int tmoutPeriod;
};

#endif // SSDCAMERA_H
