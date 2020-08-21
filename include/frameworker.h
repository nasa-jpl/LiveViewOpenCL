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
#include <QSettings>

#include "image_type.h"
#include "lvframe.h"
#include "cameramodel.h"
#include "envicamera.h"
#include "xiocamera.h"
#include "remotecamera.h"

#ifdef USE_EDT
#include "clcamera.h"
#endif

#include "twoscomplimentfilter.h"
#include "interlacefilter.h"
#include "darksubfilter.h"
#include "stddevfilter.h"
#include "meanfilter.h"
#include "constants.h"

// constexpr int FPS_FRAME_WIDTH = 10;
constexpr int MAXSAMPLES = 10;

class LVFrameBuffer;

using namespace std::chrono;

class FrameWorker : public QObject
{
    Q_OBJECT

public:
    explicit FrameWorker(QSettings *settings, QThread *worker, QObject *parent = nullptr);
    ~FrameWorker();
    void stop();
    bool running();

    CameraModel *Camera;

    void resetDir(const char *dirname);

    std::vector<float> getFrame();

    TwosComplimentFilter* TwosFilter;
    InterlaceFilter* IlaceFilter;
    DarkSubFilter* DSFilter;
    StdDevFilter* STDFilter;
    MeanFilter* MEFilter;
    std::vector<float> getDSFrame();
    std::vector<float> getSDFrame();
    std::vector<float> getSNRFrame();
    uint32_t* getHistData();
    float* getSpectralMean();
    float* getSpatialMean();
    float* getFrameFFT();

    void saveFrames(save_req_t req);

    void setCenter(double Xcoord, double Ycoord);
    QPointF* getCenter();
    void setPlotMode(LV::PlotMode pm);
    void collectMask();
    void stopCollectingMask();
    void setMaskSettings(QString mask_name, quint64 avg_frames);

    int getFrameWidth() const { return frWidth; }
    int getFrameHeight() const { return frHeight; }
    int getDataHeight() const { return dataHeight; }
    camera_t getCameraType() const { return cam_type; }

    uint32_t getStdDevN();
    double getFramePeriod();

    inline void compute_snr(LVFrame *new_frame);

    volatile bool pixRemap;
    volatile bool is16bit;
    volatile bool interlace;
    QSettings *settings;
    QPointF bottomRight;
    QPointF topLeft;
    double fps = 0;

signals:
    void finished();
    void error(const QString &error);
    void updateFPS(double fps);
    void startSaving();
    void doneSaving();
    void crosshairChanged(const QPointF &coord);

public slots:
    void reportTimeout();
    void captureFrames();
    void captureDSFrames();
    void captureSDFrames();
    void reportFPS();
    void captureFramesRemote(const save_req_t &new_req);
    void applyMask(const QString &fileName);
    void setStdDevN(int new_N);
    void setFramePeriod(double period);

private:
    QThread *thread;
    LVFrameBuffer *lvframe_buffer;
    void delay(int64_t msecs);
    std::vector<uint16_t> (FrameWorker::*p_getSaveFrame)();
    std::vector<uint16_t> getBILSaveFrame();
    std::vector<uint16_t> getBIPSaveFrame();
    void convertBSQ(save_req_t req);

    volatile LV::PlotMode plotMode;
    bool saving;
    volatile bool isRunning;
    bool isTimeout; // confusingly, isRunning is the acqusition state, isTimeout just says whether frames are currently coming across the bus.
    std::atomic<int64_t> count;
    uint64_t count_prev;
    int frWidth, frHeight, dataHeight;
    size_t frSize;
    camera_t cam_type;

    uint32_t stddev_N; // controls standard deviation history window

    std::queue<uint16_t*> frame_fifo;
    std::atomic<uint_fast32_t> save_count;
    unsigned int save_num_avgs;

    QPointF centerVal;

    std::queue<save_req_t> SaveQueue;

    QString mask_file;
    quint64 avgd_frames;

    int tickindex = 0;
    int ticksum = 0;
    std::array<int, MAXSAMPLES> ticklist;
    double frame_period_ms = 0;
    // std::mutex time_index_lock;
    // size_t time_index{0};
    // std::mutex time_mutex;
    // std::array<double, FPS_FRAME_WIDTH> time;

    // uint16_t windows_since_frame = 0;
    //Tune for FPS update speed.  In general lower will give quicker updating, less granularity at high framerates, more granularity at low framerates
    //(where high framerate is framerates with period faster than this value, and low framerate is period lower than this value).  More computation time used
    //for lower numbers as well.
    // constexpr static uint16_t frame_period = 10;
};

#endif // FRAMEWORKER_H
