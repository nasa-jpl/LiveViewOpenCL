#include "meanfilter.h"
#include <cmath>

MeanFilter::MeanFilter(unsigned int frame_width, unsigned int frame_height)
    : usingDSF(true), frWidth(frame_width), frHeight(frame_height) {}

MeanFilter::~MeanFilter() {}

void MeanFilter::compute_mean(LVFrame *frame, QPointF topLeft, QPointF bottomRight, bool use_DSF)
{
    unsigned int r, c;
    float nSamps = bottomRight.x() - topLeft.x();
    float nBands = bottomRight.y() - topLeft.y();
    usingDSF = use_DSF;

    for (r = 0; r < frHeight; r++) {
         frame->spectral_mean[r] = 0;
    }
    for (c = 0; c < frWidth; c++) {
        frame->spatial_mean[c] = 0;
    }

    for (r = 0; r < frHeight; r++) {
        for (c = topLeft.x(); c < bottomRight.x(); c++) {
            frame->spectral_mean[r] += (float)frame->raw_data[r * frWidth + c];
        }
    }
    for (r = topLeft.y(); r < bottomRight.y(); r++) {
        for (c = 0; c < frWidth; c++) {
            frame->spatial_mean[c] += (float)frame->raw_data[r * frWidth + c];
        }
    }
    for (r = 0; r < frHeight; r++) {
        frame->spectral_mean[r] /= nSamps;
    }

    for (c = 0; c < frWidth; c++) {
        frame->spatial_mean[c] /= nBands;
    }
}
