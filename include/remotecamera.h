#ifndef REMOTECAMERA_H
#define REMOTECAMERA_H

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
#include <QTcpSocket>

#include "alphanum.hpp"

#include "osutils.h"
#include "cameramodel.h"
#include "constants.h"
#include "lvframe.h"

#define TIMEOUT_DURATION 100

using namespace std::chrono;

class RemoteCamera : public CameraModel
{
    Q_OBJECT

public:
    RemoteCamera(int frWidth = 640,
              int frHeight = 480,
              int dataHeight = 480,
              int Descriptor = 0,
              QObject *parent = nullptr);
    ~RemoteCamera();

    // No setDir function because the only source is the server

    virtual uint16_t* getFrame();

public slots:
    void SocketStateChanged(QTcpSocket::SocketState state = QTcpSocket::UnconnectedState);
private:
    // Not including for now because I want an unbuffered system
//    std::string getFname();
//    void readFile();
//    void readLoop();

    QTcpSocket *socket;

    bool is_reading; // Flag that is true while reading from a directory
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
};

#endif // REMOTECAMERA_H
