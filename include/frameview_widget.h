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
    explicit frameview_widget(FrameWorker *fw, image_t image_type, QWidget *parent = NULL);
    ~frameview_widget();

public slots:
    void handleNewFrame();
    void setScrollBoth();
    void setScrollX();
    void setScrollY();
    void drawCrosshair(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event);
    void hideCrosshair(bool hide);
    virtual void rescaleRange();

private:
    float* (FrameWorker::*p_getFrame)();
    image_t image_type;

    QCPColorMap *colorMap;
    QCPColorMapData *colorMapData;
    QCPColorScale *colorScale;

    QCPItemRect *crosshairX;
    QCPItemRect *crosshairY;

    QLabel* fpsLabel;
    QTime fpsclock;
    unsigned int count;
    volatile double fps;
    QString fps_string;
};

#endif // FRAMEVIEW_WIDGET_H
