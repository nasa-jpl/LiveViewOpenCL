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
    void setOverlayPlot(image_t image_type_overlay);

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

    QComboBox *overlayPlotModeBox;

    QCheckBox *plotModeCheckbox;

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

private slots:
    void mouse_down(QMouseEvent *event);
    void mouse_move(QMouseEvent *event);
    void mouse_up(QMouseEvent *event);
};

#endif // FRAMEVIEW_WIDGET_H
