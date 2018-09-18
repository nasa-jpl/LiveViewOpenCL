#ifndef MEANFILTER_H
#define MEANFILTER_H

#include <stdint.h>

#include <QPointF>
#include <QDebug>
#include <vector>

#include "lvframe.h"
#include "sliding_dft.h"

class MeanFilter
{
public:
    MeanFilter(unsigned int frame_width, unsigned int frame_height);
    ~MeanFilter();

    void compute_mean(LVFrame *frame, QPointF topLeft, QPointF bottomRight,
                      LV::PlotMode pm, bool cam_running);
    bool dftReady();

private:
    float (MeanFilter::*p_getPixel)(uint32_t);
    float getRawPixel(uint32_t index);
    float getDSFPixel(uint32_t index);
    float getSNRPixel(uint32_t index);

    LVFrame *curFrame;

    SlidingDFT<float, FFT_INPUT_LENGTH> dft;
    bool dft_ready_read;

    unsigned int frWidth;
    unsigned int frHeight;
};


#endif // MEANFILTER_H
