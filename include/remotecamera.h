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
    virtual ~RemoteCamera();

    virtual bool start();
    virtual uint16_t* getFrame();
    void SocketRead();

public slots:
    void SocketStateChanged(QTcpSocket::SocketState state = QTcpSocket::UnconnectedState);
private:

    QTcpSocket *socket;

    bool is_receiving; // Flag that is true while requesting a frame
    bool is_connected; // Flag that is true when server is connected
    std::ifstream dev_p;
    std::string ifname;
    std::string data_dir;
    std::streampos bufsize;
    const int nFrames;
    size_t framesize;
    const int headsize;
    int socket_descriptor;

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
