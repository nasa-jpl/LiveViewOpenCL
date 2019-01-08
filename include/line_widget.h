#ifndef LINE_WIDGET_H
#define LINE_WIDGET_H

#include "lvtabapplication.h"
#include "image_type.h"

class line_widget : public LVTabApplication
{
    Q_OBJECT
public:
    explicit line_widget(FrameWorker *fw, image_t image_t, QWidget *parent = nullptr);
    ~line_widget() = default;

    image_t image_type;

public slots:
    void handleNewFrame();
    void lineScrolledY(const QCPRange &newRange);
    void rescaleRange();
    void updatePlotTitle(const QPointF &coord);
    void setTracer(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event);
    void moveCallout(QMouseEvent *e);
    void hideCallout(bool toggled);
    void setPlotMode(int pm);
    void setPlotMode(LV::PlotMode pm);
    void setDarkMode(bool dm);

private:
    QVector<double> (line_widget::*p_getLine)(QPointF);
    std::vector<float> (FrameWorker::*p_getFrame)();
    QVector<double> getSpectralLine(QPointF coord);
    QVector<double> getSpatialLine(QPointF coord);
    QVector<double> getSpectralMean(QPointF coord);
    QVector<double> getSpatialMean(QPointF coord);
    QVector<double> x, y;

    QCPTextElement *plotTitle;

    QComboBox *plotModeBox;
    QCheckBox *hideTracer;

    QCPItemTracer *tracer;
    QCPItemText *callout;
    QCPItemLine *arrow;

    int xAxisMax;
};

#endif // LINE_WIDGET_H
