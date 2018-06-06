#ifndef MEANFILTER_H
#define MEANFILTER_H

#include <stdint.h>

#include <QPointF>
#include <QDebug>
#include <deque>
#include <vector>

#include "lvframe.h"

class MeanFilter
{
public:
    MeanFilter(unsigned int frame_width, unsigned int frame_height);
    ~MeanFilter();

    void compute_mean(LVFrame *frame, QPointF topLeft, QPointF bottomRight, bool useDSF);

private:
    float (MeanFilter::*p_getPixel)(uint32_t);
    float getRawPixel(uint32_t index);
    float getDSFPixel(uint32_t index);
    std::deque<double> frame_means;

    LVFrame *curFrame;

    unsigned int frWidth;
    unsigned int frHeight;
};


#endif // MEANFILTER_H
