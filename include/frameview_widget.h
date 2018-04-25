#ifndef FRAMEVIEW_WIDGET_H
#define FRAMEVIEW_WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QVBoxLayout>

#include <stdint.h>
#include <functional>

#include "lvtabapplication.h"
#include "image_type.h"
#include "frameworker.h"
#include "qcustomplot/qcustomplot.h"
#include "constants.h"

class frameview_widget : public LVTabApplication
{
    Q_OBJECT
public:
    explicit frameview_widget(image_t image_type, FrameWorker* fw, QWidget *parent = NULL);
    ~frameview_widget();

public slots:
    void handleNewFrame();
    void colorMapScrolledY(const QCPRange &newRange);
    void colorMapScrolledX(const QCPRange &newRange);
    void setScrollY(bool Xenabled);
    void setScrollX(bool Yenabled);
    virtual void rescaleRange();

private:
    uint16_t* (FrameWorker::*p_getFrame)();
    image_t image_type;
    QTimer rendertimer;
    FrameWorker* frame_handler;

    QCustomPlot* qcp;
    QCPColorMap* colorMap;
    QCPColorMapData *colorMapData;
    QCPColorScale* colorScale;

    QLabel* fpsLabel;
    QTime fpsclock;
    unsigned int count;
    volatile double fps;
    QString fps_string;

    QCheckBox* zoomXCheck;
    QCheckBox* zoomYCheck;
    bool scrollXenabled;
    bool scrollYenabled;

    QGridLayout* layout;

    uint16_t frHeight;
    uint16_t frWidth;
};

#endif // FRAMEVIEW_WIDGET_H
