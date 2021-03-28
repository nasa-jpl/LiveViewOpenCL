#ifndef FRAMEVIEW_WIDGET_H
#define FRAMEVIEW_WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QGroupBox>
#include <QRadioButton>
#include <QSettings>
#include <atomic>         // 3-15-21 frame-line-control enhancement

#include <stdint.h>
#include <functional>
#include "lvtabapplication.h"

#include "image_type.h"
#include "constants.h"


class frameview_widget : public LVTabApplication
{
    Q_OBJECT
public:
    explicit frameview_widget(FrameWorker *fw,
                              image_t image_type,
                              QSettings *set,
                              QWidget *parent = nullptr);
    ~frameview_widget() = default;

public slots:
    void handleNewFrame();
    void drawCrosshair(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event);
    void hideCrosshair(bool hide);
    void showTooltip(bool show);
    void rescaleRange();
    void reportFPS();
    void setPlotMode(bool checked);
    QCPColorMap* getColorMap();


public:
    //
    // 2-28-21 image-line-control
    void displayFrameLines( frameDataFile *LVData );
    frameLineData getFrameLine( int i, frameDataFile fData );
    void displaySingleFrameLine( int i, size_t frameSizeInPixel, frameLineData line );

    // void *cboxParent;    // 3-15-21 frame-line-control enhancement
    void updateFrameLineControlStatus( bool status );
    bool IsFrameLineControlEnabled( void );   // 3-19-21 mod. 

    void forwardToNextFrameLine( bool nextLine ) ;
    bool IsTimeToDisplayNextFrameLine( void ); // 3-19-21 mod. 

    void resetFrameLineDisplay( bool resetNow );
    bool IsTimeToResetFrameLineDisplay( void ); // 3-19-21 mod. 

    //
    // PK 3-20-21 attempts to fix Linux filename update issue !!
    void displayFrameFilename( const std::string filename );

private:
    //
    // EMITFPIED-331
    // PK new mouse feature 11-16-20
    FrameWorker *frameWorkerParent;

    inline void setDarkMode();

    std::vector<float> (FrameWorker::*p_getFrame)();
    
    // std::vector<float> (FrameWorker::*CameraFrameControlMgr();
    image_t image_type;

    QCPColorMap *colorMap;
    QCPColorMapData *colorMapData;
    QCPColorScale *colorScale;

    QCPItemRect *crosshairX;
    QCPItemRect *crosshairY;

    QCPItemRect *tlBox;
    QCPItemRect *trBox;
    QCPItemRect *blBox;
    QCPItemRect *brBox;

    QLabel* fpsLabel;
    QTimer fpsclock;
    volatile unsigned int count;
    unsigned int count_prev;
    double fps;
    QString fps_string;

    QSettings *settings;
    bool boxes_enabled = false;
    bool dragging_vertical_box = false;
    bool dragging_horizontal_box = false;
    bool show_tooltip = false;

    double loBoundX;
    double hiBoundX;

    double loBoundY;
    double hiBoundY;


    // 3-15-21 frame-line-control enhancement ...
    bool frameLineControlEnabled;  
    bool displayNextFrameLine;
    bool frameLineDisplayReset;

    std::atomic<int> frameLineIndex;

    // ... 3-15-21 frame-line-control enhancement

private slots:
    void mouse_down(QMouseEvent *event);
    void mouse_move(QMouseEvent *event);
    void mouse_up(QMouseEvent *event);


signals:   // PK 3-4-21 image-line-control
    void updateFrameFilename( const std::string filename );
    void updateFrameLineInfo();

};

#endif // FRAMEVIEW_WIDGET_H
