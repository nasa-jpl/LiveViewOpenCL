#include "darksubfilter.h"

DarkSubFilter::DarkSubFilter(int frame_width, int frame_height) :
    mask_collected(false), frWidth(frame_width), frHeight(frame_height)
{
    frSize = frWidth * frHeight;
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
    std::fill(mask.begin(), mask.end(), 0.0);
    std::fill(mask_accum.begin(), mask_accum.end(), 0.0);
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

void DarkSubFilter::dark_subtract(uint16_t* in_frame, uint16_t* out_frame)
{
    for (unsigned int i = 0; i < frSize; i++) {
        out_frame[i] = (uint16_t)((float)in_frame[i] - mask[i]);
    }
}

void DarkSubFilter::dsf_callback(uint16_t *in_frame, uint16_t *out_frame)
{
    if (mask_collected) {
        dark_subtract(in_frame, out_frame);
    } else {
        mask_mutex.lock();
        for (unsigned int i = 0; i < frSize; i++) {
            out_frame[i] = in_frame[i];
        }
        collect_mask(in_frame);
        mask_mutex.unlock();
    }
}

