#ifndef MEANFILTER_H
#define MEANFILTER_H

#include <stdint.h>

#include <QPointF>
#include <QDebug>

#include "lvframe.h"

class MeanFilter
{
public:
    MeanFilter(unsigned int frame_width, unsigned int frame_height);
    virtual ~MeanFilter();

    void compute_mean(LVFrame *frame, QPointF topLeft, QPointF bottomRight, bool use_DSF);

private:
    bool usingDSF;

    unsigned int frWidth;
    unsigned int frHeight;

};


#endif // MEANFILTER_H
