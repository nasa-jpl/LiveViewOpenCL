#include "meanfilter.h"

MeanFilter::MeanFilter(unsigned int frame_width, unsigned int frame_height)
    : usingDSF(true), frWidth(frame_width), frHeight(frame_height) {}

MeanFilter::~MeanFilter() {}

void MeanFilter::compute_mean(LVFrame *frame, QPointF topLeft, QPointF bottomRight, bool use_DSF)
{
    float *new_image;
    unsigned int r, c;
    float nSamps = topLeft.x() - bottomRight.x();
    float nBands = topLeft.y() - bottomRight.y();
    usingDSF = use_DSF;
    new_image = frame->dsf_data;

    for (r = 0; r < frHeight; r++) {
        for (c = topLeft.x(); c < bottomRight.x(); c++) {
            frame->spectral_mean[r] += new_image[r * frWidth + c];
        }
    }
    for (r = topLeft.y(); r < bottomRight.y(); r++) {
        for (c = 0; c < frWidth; c++) {
            frame->spatial_mean[c] += new_image[r * frWidth + c];
        }
    }
    for (r = 0; r < frHeight; r++) {
        frame->spectral_mean[r] /= nSamps;
    }
    for (c = 0; c < frWidth; c++) {
        frame->spatial_mean[c] /= nBands;
    }
}
