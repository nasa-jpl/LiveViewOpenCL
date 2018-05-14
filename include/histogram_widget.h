#ifndef HISTOGRAM_WIDGET_H
#define HISTOGRAM_WIDGET_H

#include <QVBoxLayout>
#include <QTimer>

#include "lvtabapplication.h"
#include "stddevfilter.h"

class histogram_widget : public LVTabApplication
{
    Q_OBJECT
public:
    explicit histogram_widget(FrameWorker *fw, QWidget *parent = NULL);
    ~histogram_widget();

public slots:
    void handleNewFrame();
    void histogramScrolledX(const QCPRange &newRange);
    void histogramScrolledY(const QCPRange &newRange);
    virtual void rescaleRange();
    void resetRange();

private:
    FrameWorker *frame_handler;
    QTimer renderTimer;

    QVBoxLayout *qvbl;

    QCustomPlot *qcp;
    QCPBars *histogram;

    QVector<double> hist_bins;
    QVector<double> hist_data;
    unsigned int count = 0;

    unsigned int frHeight;
    unsigned int frWidth;
};

#endif // HISTOGRAM_WIDGET_H
