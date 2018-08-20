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
        QWidget(parent), dataMax(float(UINT16_MAX)), dataMin(0.0),
        isPrecise(false), frame_handler(fw)
    {
        frHeight = frame_handler->getFrameHeight();
        frWidth = frame_handler->getFrameWidth();
        qcp = new QCustomPlot(this);
        qcp->setNotAntialiasedElement(QCP::aeAll);

        lowerRangeBoundX = 0;
        lowerRangeBoundY = 0;
    }

    virtual ~LVTabApplication() {}

    double getCeiling() { return ceiling; }
    double getFloor() { return floor; }
    double getDataMax() { return dataMax; }
    double getDataMin() { return dataMin; }
    bool isPrecisionMode() { return isPrecise; }

    virtual void setCeiling(int c) {
        ceiling = (double)c;
        rescaleRange();
    }
    virtual void setFloor(int f) {
        floor = (double)f;
        rescaleRange();
    }

    virtual void setPrecision(bool precise) {
        isPrecise = precise;
        if (isPrecise) {
            dataMin = -2000.0;
            dataMax = 2000.0;
            double c = ceiling > dataMax ? dataMax : ceiling;
            setCeiling((int)c);
            double f = floor < dataMin ? dataMin : floor;
            setFloor((int)f);
        } else {
            dataMin = 0.0;
            dataMax = UINT16_MAX;
            double c = ceiling > dataMax ? dataMax : ceiling;
            setCeiling((int)c);
            double f = floor < dataMin ? dataMin : floor;
            setFloor((int)f);
        }
    }

public slots:
    virtual void setFloorPos(int minPos) {
        double f = dataMax * ((double)minPos / 100.0);
        setFloor((int)f);
    }

    virtual void setCeilingPos(int maxPos) {
        double c = dataMax * ((double)maxPos / 100.0);
        setCeiling((int)c);
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

    volatile double ceiling;
    volatile double floor;

    bool isPrecise;

    FrameWorker *frame_handler;
    QTimer renderTimer;

    QCustomPlot *qcp;

    unsigned int frWidth;
    unsigned int frHeight;

    double lowerRangeBoundX;
    double upperRangeBoundX;
    double lowerRangeBoundY;
    double upperRangeBoundY;
};


#endif // LVTABAPPLICATION_H
