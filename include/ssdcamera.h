#ifndef SSDCAMERA_H
#define SSDCAMERA_H

#include <stdlib.h>
#include <cstring>
#include <fstream>
#include <deque>
#include <vector>
#include <array>
#include <algorithm>
#include <chrono>

#include <QDebug>
#include <QDir>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>

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
    ~SSDCamera() = default;

    virtual bool start();
    virtual void setDir(const char *dirname);

    virtual uint16_t* getFrame();

private:
    std::string getFname();
    void readFile();
    void readLoop();

    bool is_reading;
    bool initialize;
    std::ifstream dev_p;
    std::string ifname;
    std::string data_dir;
    std::streampos bufsize;
    const unsigned int nFrames;
    unsigned int framesize;
    const unsigned int headsize;

    size_t image_no;
    std::vector<std::string> xio_files;
    std::deque< std::vector<uint16_t> > frame_buf;
    std::vector<unsigned char> header;
    std::vector<uint16_t> dummy;
    std::vector<uint16_t> temp_frame;

    QFuture<void> readLoopFuture;
    int64_t tmoutPeriod;
};

#endif // SSDCAMERA_H
