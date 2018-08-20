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
    explicit histogram_widget(FrameWorker *fw, QWidget *parent = nullptr);
    ~histogram_widget();

public slots:
    void handleNewFrame();
    void histogramScrolledY(const QCPRange &newRange);
    void rescaleRange();
    void resetRange();

private:
    QCPBars *histogram;

    QVector<double> hist_bins;
    QVector<double> hist_data;
};

#endif // HISTOGRAM_WIDGET_H
