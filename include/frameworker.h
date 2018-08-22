#ifndef FRAMEWORKER_H
#define FRAMEWORKER_H

#include <chrono>
#include <queue>

#include <QMessageBox>
#include <QPointF>
#include <QTime>
#include <QTimer>
#include <QCoreApplication>
#include <QThread>
#include <QtConcurrent/QtConcurrentRun>

#include "image_type.h"
#include "lvframe.h"
#include "cameramodel.h"
#include "debugcamera.h"
#include "ssdcamera.h"
#if !(__APPLE__ || __MACH__)
#include "clcamera.h"
#endif
#include "darksubfilter.h"
#include "stddevfilter.h"
#include "meanfilter.h"
#include "constants.h"

class LVFrameBuffer;

using namespace std::chrono;

struct save_req_t
{
    std::string file_name;
    uint64_t nFrames;
    uint64_t nAvgs;
};

class FrameWorker : public QObject
{
    Q_OBJECT

public:
    explicit FrameWorker(QThread *worker, QObject *parent = nullptr);
    ~FrameWorker();
    void stop();
    bool running();

    void resetDir(const char *dirname);

    std::vector<float> getFrame();

    DarkSubFilter* DSFilter;
    StdDevFilter* STDFilter;
    MeanFilter* MEFilter;
    std::vector<float> getDSFrame();
    std::vector<float> getSDFrame();
    uint32_t* getHistData();
    float* getSpectralMean();
    float* getSpatialMean();
    float* getFrameFFT();

    void saveFrames(std::string frame_fname, uint64_t num_frames);

    void setCenter(double Xcoord, double Ycoord);
    QPointF* getCenter();
    void setDSF(bool toggled);
    void collectMask();
    void stopCollectingMask();
    void setMaskSettings(QString mask_name, quint64 avg_frames);

    uint16_t getFrameWidth() const { return frWidth; }
    uint16_t getFrameHeight() const { return frHeight; }
    uint16_t getDataHeight() const { return dataHeight; }
    camera_t getCameraType() const { return cam_type; }

signals:
    void finished();
    void error(const QString &error);
    void updateFPS(float fps);
    void startSaving();
    void doneSaving();
    void crosshairChanged(const QPointF &coord);

public slots:
    void reportTimeout();
    void captureFrames();
    void captureDSFrames();
    void captureSDFrames();
    void reportFPS();
    void captureFramesRemote(const QString &fileName, const quint64 &nFrames, const quint64 &nAvgs);
    void applyMask(const QString &fileName);

private:
    QThread *thread;
    LVFrameBuffer *lvframe_buffer;
    CameraModel *Camera;
    void delay(int msecs);

    bool pixRemap;
    volatile bool useDSF;
    bool saving;
    volatile bool isRunning;
    bool isTimeout; // confusingly, isRunning is the acqusition state, isTimeout just says whether frames are currently coming across the bus.
    std::atomic<uint64_t> count;
    uint64_t count_prev;
    unsigned int frWidth, frHeight, dataHeight, frSize;
    camera_t cam_type;

    uint32_t stddev_N; // controls standard deviation history window

    std::queue<uint16_t*> frame_fifo;
    std::atomic<uint_fast32_t> save_count;
    unsigned int save_num_avgs;

    QPointF centerVal;

    std::queue<save_req_t> SaveQueue;

    QString mask_file;
    quint64 avgd_frames;

};

#endif // FRAMEWORKER_H
