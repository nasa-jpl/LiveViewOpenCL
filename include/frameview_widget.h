#ifndef FRAMEVIEW_WIDGET_H
#define FRAMEVIEW_WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QGroupBox>
#include <QRadioButton>
#include <QSettings>

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
    ~frameview_widget();

public slots:
    void handleNewFrame();
    void drawCrosshair(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event);
    void hideCrosshair(bool hide);
    void rescaleRange();
    void reportFPS();
    void setPlotMode(bool checked);
    QCPColorMap* getColorMap();

private:
    inline void setDarkMode();
    std::vector<float> (FrameWorker::*p_getFrame)();
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

    double s_0;
    double s_1;

    double l_0;
    double l_1;

private slots:
    void mouse_down(QMouseEvent *event);
    void mouse_move(QMouseEvent *event);
    void mouse_up(QMouseEvent *event);
};

#endif // FRAMEVIEW_WIDGET_H
