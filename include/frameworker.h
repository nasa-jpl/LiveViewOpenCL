#ifndef FRAMEWORKER_H
#define FRAMEWORKER_H

#include <chrono>

#include <QPointF>
#include <QTime>
#include <QTimer>
#include <QCoreApplication>
#include <QThread>

#include "image_type.h"
#include "lvframe.h"
#include "cameramodel.h"
#include "debugcamera.h"
#include "ssdcamera.h"
#include "clcamera.h"
#include "darksubfilter.h"
#include "stddevfilter.h"
#include "constants.h"

class LVFrameBuffer;

using namespace std::chrono;

class FrameWorker : public QObject
{
    Q_OBJECT

public:
    explicit FrameWorker(QThread *worker, QObject *parent = NULL);
    virtual ~FrameWorker();
    void stop();
    bool running();

    float* getFrame();

    DarkSubFilter* DSFilter;
    StdDevFilter* STDFilter;
    float* getDSFrame();
    float* getSDFrame();
    uint32_t* getHistData();

    void saveFrames(std::string fname_out, unsigned int num_avgs, unsigned int num_frames);

    void setCenter(double Xcoord, double Ycoord);
    QPointF* getCenter();

    uint16_t getFrameWidth() const { return frWidth; }
    uint16_t getFrameHeight() const { return frHeight; }
    uint16_t getDataHeight() const { return dataHeight; }
    camera_t getCameraType() const { return cam_type; }

signals:
    void finished();
    void error(const QString &error);
    void updateFPS(float fps);
    void doneSaving();
    void crosshairChanged(const QPointF &coord);

public slots:
    void reportTimeout();
    void captureFrames();
    void captureDSFrames();
    void captureSDFrames();
    void resetDir(const char *dirname);
    void reportFPS();

private:
    QThread *thread;
    LVFrameBuffer *lvframe_buffer;
    CameraModel *Camera;
    void delay(int msecs);

    bool pixRemap;
    bool isRunning;
    std::atomic<bool> isTimeout; // confusingly, isRunning is the acqusition state, isTimeout just says whether frames are currently coming across the bus.
    volatile unsigned int count;
    unsigned int count_prev;
    unsigned int frWidth, frHeight, dataHeight, frSize;
    camera_t cam_type;

    uint32_t stddev_N; // controls standard deviation history window

    std::list<uint16_t*> saveframe_list;
    std::atomic<uint_fast32_t> save_framenum;
    std::atomic<uint_fast32_t> save_count;
    unsigned int save_num_avgs;

    QPointF centerVal;

};

#endif // FRAMEWORKER_H
