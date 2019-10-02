#ifndef LVTABAPPLICATION_H
#define LVTABAPPLICATION_H

#include <stdint.h>
#include <QWidget>
#include <QTimer>
#include <QDebug>

#include "frameworker.h"
#include "qcustomplot/qcustomplot.h"

class LVTabApplication : public QWidget
{
    Q_OBJECT
public:
    LVTabApplication(FrameWorker *fw, QWidget *parent = nullptr) :
        QWidget(parent), dataMax(UINT16_MAX), dataMin(0.0),
        timeout_display(false), isPrecise(false), frame_handler(fw)
    {
        frHeight = frame_handler->getFrameHeight();
        frWidth = frame_handler->getFrameWidth();
        qcp = new QCustomPlot(this);
        qcp->setNotAntialiasedElement(QCP::aeAll);

        lowerRangeBoundX = 0;
        lowerRangeBoundY = 0;

        connect(frame_handler->Camera, &CameraModel::started, this, [=]() {
           this->renderTimer.start(FRAME_DISPLAY_PERIOD_MSECS);
        });
    }

    virtual ~LVTabApplication() {}

    double getCeiling() { return ceiling; }
    double getFloor() { return floor; }
    double getDataMax() { return dataMax; }
    double getDataMin() { return dataMin; }
    bool isPrecisionMode() { return isPrecise; }

    virtual void setCeiling(double c) {
        ceiling = c;
        rescaleRange();
    }
    virtual void setFloor(double f) {
        floor = f;
        rescaleRange();
    }

    virtual void setPrecision(bool precise) {
        isPrecise = precise;
        if (isPrecise) {
            dataMin = -2000.0;
            dataMax = 2000.0;
            double c = ceiling > dataMax ? dataMax : ceiling;
            setCeiling(c);
            double f = floor < dataMin ? dataMin : floor;
            setFloor(f);
        } else {
            dataMin = 0.0;
            dataMax = UINT16_MAX;
            double c = ceiling > dataMax ? dataMax : ceiling;
            setCeiling(c);
            double f = floor < dataMin ? dataMin : floor;
            setFloor(f);
        }
    }

public slots:
    virtual void setFloorPos(double minPos) {
        double f = dataMax * (minPos / 99.0);
        setFloor(f);
    }

    virtual void setCeilingPos(double maxPos) {
        double c = dataMax * (maxPos / 99.0);
        setCeiling(c);
    }

    virtual void rescaleRange() = 0;

    virtual void graphScrolledX(const QCPRange &newRange)
    {
        QCPRange boundedRange = newRange;
        if (boundedRange.size() > upperRangeBoundX - lowerRangeBoundX) {
            boundedRange = QCPRange(lowerRangeBoundX, upperRangeBoundX);
        } else {
            double oldSize = boundedRange.size();
            if (boundedRange.lower < lowerRangeBoundX) {
                boundedRange.lower = lowerRangeBoundX;
                boundedRange.upper = lowerRangeBoundX + oldSize;
            }
            if (boundedRange.upper > upperRangeBoundX) {
                boundedRange.lower = upperRangeBoundX - oldSize;
                boundedRange.upper = upperRangeBoundX;
            }
        }
        qcp->xAxis->setRange(boundedRange);
    }

    virtual void graphScrolledY(const QCPRange &newRange)
    {
        QCPRange boundedRange = newRange;
        if (boundedRange.size() > upperRangeBoundY - lowerRangeBoundY) {
            boundedRange = QCPRange(lowerRangeBoundY, upperRangeBoundY);
        } else {
            double oldSize = boundedRange.size();
            if (boundedRange.lower < lowerRangeBoundY) {
                boundedRange.lower = lowerRangeBoundY;
                boundedRange.upper = lowerRangeBoundY + oldSize;
            } if (boundedRange.upper > upperRangeBoundY) {
                boundedRange.lower = upperRangeBoundY - oldSize;
                boundedRange.upper = upperRangeBoundY;
            }
        }
        qcp->yAxis->setRange(boundedRange);
    }

protected:
    double dataMax;
    double dataMin;

    bool timeout_display;

    volatile double ceiling;
    volatile double floor;

    bool isPrecise;

    FrameWorker *frame_handler;
    QTimer renderTimer;

    QCustomPlot *qcp;

    int frWidth;
    int frHeight;

    double lowerRangeBoundX;
    double upperRangeBoundX;
    double lowerRangeBoundY;
    double upperRangeBoundY;
};


#endif // LVTABAPPLICATION_H
