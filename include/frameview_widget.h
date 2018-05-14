#ifndef FRAMEVIEW_WIDGET_H
#define FRAMEVIEW_WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QGroupBox>
#include <QRadioButton>

#include <stdint.h>
#include <functional>

#include "lvtabapplication.h"
#include "image_type.h"
#include "constants.h"

class frameview_widget : public LVTabApplication
{
    Q_OBJECT
public:
    explicit frameview_widget(image_t image_type, FrameWorker* fw, QWidget* parent = NULL);
    ~frameview_widget();

public slots:
    void handleNewFrame();
    void colorMapScrolledY(const QCPRange &newRange);
    void colorMapScrolledX(const QCPRange &newRange);
    void setScrollBoth();
    void setScrollX();
    void setScrollY();
    virtual void rescaleRange();

private:
    float* (FrameWorker::*p_getFrame)();
    image_t image_type;
    QTimer rendertimer;
    FrameWorker* frame_handler;

    QCustomPlot* qcp;
    QCPColorMap* colorMap;
    QCPColorMapData* colorMapData;
    QCPColorScale* colorScale;

    QLabel* fpsLabel;
    QTime fpsclock;
    unsigned int count;
    volatile double fps;
    QString fps_string;

    unsigned int frHeight;
    unsigned int frWidth;
};

#endif // FRAMEVIEW_WIDGET_H
