#include "darksubfilter.h"

DarkSubFilter::DarkSubFilter(unsigned int frame_size) :
    mask_collected(true), frSize(frame_size)
{
    mask.reserve(frSize);
    mask_accum.reserve(frSize);
}

DarkSubFilter::~DarkSubFilter()
{
    mask_collected = false;
}

void DarkSubFilter::start_mask_collection()
{
    mask_collected = false;
    nSamples = 0;
    // Did not work for some unknown reason
    // std::fill (mask.begin(), mask.end(), 0.0);
    // std::fill (mask_accum.begin(), mask_accum.end(), 0.0);
    for (unsigned int i = 0; i < frSize; i++) {
        mask[i] = 0.0;
        mask_accum[i] = 0.0;
    }

}

void DarkSubFilter::finish_mask_collection()
{
    for (unsigned int i = 0; i < frSize; i++) {
        mask[i] = mask_accum[i] / (float)nSamples;
    }
    mask_collected = true;

    qDebug("Mask collected!");
}

void DarkSubFilter::collect_mask(uint16_t* in_frame)
{
    for (unsigned int i = 0; i < frSize; i++) {
        mask_accum[i] = in_frame[i] + mask_accum[i];
    }

    nSamples++;
}

void DarkSubFilter::dark_subtract(uint16_t* in_frame, float* out_frame)
{
    for (unsigned int i = 0; i < frSize; i++) {
        out_frame[i] = (float)in_frame[i] - mask[i];
    }
}

void DarkSubFilter::dsf_callback(uint16_t *in_frame, float *out_frame)
{
    if (mask_collected) {
        dark_subtract(in_frame, out_frame);
    } else {
        mask_mutex.lock();
        for (unsigned int i = 0; i < frSize; i++) {
            out_frame[i] = (float)in_frame[i];
        }
        collect_mask(in_frame);
        mask_mutex.unlock();
    }
}

