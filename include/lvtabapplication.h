#ifndef LVTABAPPLICATION_H
#define LVTABAPPLICATION_H

#include <stdint.h>
#include <QWidget>
#include <QDebug>

#include "frameworker.h"
#include "qcustomplot/qcustomplot.h"

class LVTabApplication : public QWidget
{
    Q_OBJECT

public:
    LVTabApplication(QWidget *_parent = NULL) : parent(_parent), dataMax(float(UINT16_MAX)), dataMin(0.0), isPrecise(false) {}
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


    QWidget* parent;

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

protected:
    double dataMax;
    double dataMin;

    volatile double ceiling;
    volatile double floor;

    bool isPrecise;
};


#endif // LVTABAPPLICATION_H
