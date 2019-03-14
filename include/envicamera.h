#ifndef DEBUGCAMERA_H
#define DEBUGCAMERA_H

#include <array>
#include <deque>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>

#include <QtConcurrent/QtConcurrent>
#include <QCoreApplication>
#include <QDebug>
#include <QFuture>
#include <QTime>

#include "cameramodel.h"
#include "constants.h"
#include "lvframe.h"
#include "osutils.h"

struct ENVIData {
    int bands;        // height
    int samples;      // width
    int lines;        // num. frames
    org_t interleave; // bit organization
    int nbits;        // num. bits/pixel
};

class ENVICamera : public CameraModel
{
    Q_OBJECT

public:
    ENVICamera(int frWidth = 640,
               int frHeight = 480,
               int dataHeight = 480,
               QObject *parent = nullptr);
    ~ENVICamera();

    virtual uint16_t *getFrame();
    virtual void setDir(const char *filename);

private:
    bool readHeader(std::string hdrname);
    ENVIData HDRData;
    void readLoop();

    bool is_reading; // Flag that is true while reading from a directory
    std::ifstream dev_p;
    std::string ifname;
    std::string hdrname;
    std::streampos bufsize;
    const int framesize;
    std::deque< std::vector<uint16_t> > frame_buf;
    std::vector<uint16_t> dummy;
    std::vector<uint16_t> temp_frame;
    int curIndex;
    int nFrames;
    int chunkFrames; // frames to read per buffered chunk
    int framesRead;

    QFuture<void> readLoopFuture;
    int tmoutPeriod;
};

#endif // DEBUGCAMERA_H
