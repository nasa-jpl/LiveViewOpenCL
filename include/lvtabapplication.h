#ifndef LVTABAPPLICATION_H
#define LVTABAPPLICATION_H

#include <stdint.h>
#include <QWidget>
#include <QDebug>

class LVTabApplication : public QWidget
{
    Q_OBJECT

public:
    LVTabApplication(QWidget *_parent = NULL) : parent(_parent), dataMax(UINT16_MAX), dataMin(0) {}
    double getCeiling() { return ceiling; }
    double getFloor() { return floor; }
    double getDataMax() { return dataMax; }
    double getDataMin() { return dataMin; }

    virtual void setCeiling(int c) {
        ceiling = (double)c;
        rescaleRange();
    }
    virtual void setFloor(int f) {
        floor = (double)f;
        rescaleRange();
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
};


#endif // LVTABAPPLICATION_H
