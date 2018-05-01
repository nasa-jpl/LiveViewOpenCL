#ifndef SSDCAMERA_H
#define SSDCAMERA_H

#include <stdlib.h>
#include <string>
#include <fstream>
#include <vector>
#include <array>
#include <algorithm>

#include <QDebug>

#include "alphanum.hpp"

#include "osutils.h"
#include "cameramodel.h"
#include "constants.h"
#include "lvframe.h"

#define TIMEOUT_DURATION 100

class SSDCamera : public CameraModel
{
    Q_OBJECT

public:
    SSDCamera(const std::string search_dir,
              unsigned int frWidth = 640,
              unsigned int frHeight = 480,
              unsigned int dataHeight = 480,
              QObject *parent = NULL);
    virtual ~SSDCamera();

    virtual bool start();
    virtual void setDir(const char *dirname);

    virtual uint16_t *getFrame();

private:
    std::string getFname();
    void readFile();

    std::ifstream dev_p;
    std::string ifname;
    std::string data_dir;
    std::streampos bufsize;
    const unsigned int nFrames;
    std::atomic<unsigned int> framesize;
    const unsigned int headsize;

    size_t image_no;
    std::vector<std::string> xio_files;
    std::vector< std::vector<uint16_t> > frame_buf;
    std::vector<unsigned char> header;
    std::vector<uint16_t> dummy;
    std::atomic<uint16_t> curIndex;
};

#endif // SSDCAMERA_H
