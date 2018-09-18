#include "meanfilter.h"
#include <cmath>

MeanFilter::MeanFilter(unsigned int frame_width, unsigned int frame_height)
    : dft_ready_read(false), frWidth(frame_width), frHeight(frame_height)
{}

MeanFilter::~MeanFilter() {}

void MeanFilter::compute_mean(LVFrame *frame, QPointF topLeft, QPointF bottomRight,
                              LV::PlotMode pm, bool cam_running)
{
    unsigned int r, c, k;
    float nSamps = bottomRight.x() - topLeft.x();
    float nBands = bottomRight.y() - topLeft.y();
    float frame_mean = 0.0;
    float data_point = 0.0;

    switch (pm) {
    case LV::pmRAW:
        p_getPixel = &MeanFilter::getRawPixel;
        break;
    case LV::pmDSF:
        p_getPixel = &MeanFilter::getDSFPixel;
        break;
    case LV::pmSNR:
        p_getPixel = &MeanFilter::getSNRPixel;
        break;
    }
    curFrame = frame;

    for (r = 0; r < frHeight; r++) {
         frame->spectral_mean[r] = 0;
    }
    for (c = 0; c < frWidth; c++) {
        frame->spatial_mean[c] = 0;
    }

    for (r = 0; r < frHeight; r++) {
        for (c = 0; c < frWidth; c++) {
            data_point = (this->*p_getPixel)(r * frWidth + c);
            if (c > topLeft.x() && c < bottomRight.x()) {
                frame->spectral_mean[r] += data_point;
            }
            if (r > topLeft.y() && r < bottomRight.y()) {
                frame->spatial_mean[c] += data_point;
            }
            frame_mean += data_point;
        }
    }
    frame_mean /= (frWidth * frHeight);

    dft_ready_read = dft.update(frame_mean);
    if (dft_ready_read && cam_running) {
        dft.get(frame->frame_fft);
    } else {
        for (k = 0; k < FFT_INPUT_LENGTH; k++) {
            frame->frame_fft[k] = 0.0f;
        }
    }

    for (r = 0; r < frHeight; r++) {
        frame->spectral_mean[r] /= nSamps;
    }

    for (c = 0; c < frWidth; c++) {
        frame->spatial_mean[c] /= nBands;
    }
}

float MeanFilter::getRawPixel(uint32_t index)
{
    return curFrame->raw_data[index];
}

float MeanFilter::getDSFPixel(uint32_t index)
{
    return curFrame->dsf_data[index];
}

float MeanFilter::getSNRPixel(uint32_t index)
{
    return curFrame->snr_data[index];
}

bool MeanFilter::dftReady()
{
    return dft_ready_read;
}
