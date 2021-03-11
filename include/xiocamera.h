#ifndef XIOCAMERA_H
#define XIOCAMERA_H

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



class XIOCamera : public CameraModel
{
    Q_OBJECT

public:
    XIOCamera(int frWidth = 640,
              int frHeight = 480,
              int dataHeight = 480,
              QObject *parent = nullptr);
    ~XIOCamera();

    virtual void setDir(const char *dirname);

    // EMITFPIED-331
    virtual void suspendFrameAcquisition( bool status );

    virtual uint16_t* getFrame();

    frameLineData parseLineData( std::vector<uint16_t> line );

    void dumpFrameFileData( frameDataFile *f );
    
private:
    std::string getFname();
    //
    // PK 3-6-21 debug-LV-hang
    bool readFile(); 


    void readLoop();


    bool is_reading; // Flag that is true while reading from a directory

    // EMITFPIED-331
    bool frameAcquisitionSuspended;
    int  frameAcquisitionCount;
    
    // PK 1-13-21 added ... Foward button support 
    int  getFrameAcquisitionCount( void );
    void setFrameAcquisitionCount( int count );


    std::ifstream dev_p;
    std::string ifname;
    std::string data_dir;
    std::streampos bufsize;
    const int nFrames;
    size_t framesize;
    const int headsize;

    size_t image_no;
    std::vector<std::string> xio_files;
    std::deque< std::vector<uint16_t> > frame_buf;
    std::vector<unsigned char> header;
    std::vector<uint16_t> dummy;
    std::vector<uint16_t> temp_frame;

    QFuture<void> readLoopFuture;
    int tmoutPeriod;

    // PK 2-11-21 image-line-control
    // std::vector<frameFileData> frameDataFileList;
    std::vector<frameDataFile> frameDataFileList;
    frameDataFile frameData;
};

#endif // XIOCAMERA_H
