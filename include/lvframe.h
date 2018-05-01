#ifndef LVFRAME_H
#define LVFRAME_H

#include <QtGlobal>

#include "constants.h"

struct LVFrame
{
    uint16_t* raw_data;
    float* dsf_data;
    // float* stdev_data;

    LVFrame(const unsigned int frame_size)
    {
        try {
            raw_data = new uint16_t[frame_size];
            dsf_data = new float[frame_size];
            // stdev_data = new float[width * height];
        } catch (std::bad_alloc&) {
            qFatal("Not enough memory to allocate frame buffer.");
            throw;
        }

    }
    ~LVFrame()
    {
        delete raw_data;
        delete dsf_data;
        // delete stdev_data;
    }
};

#endif // LVFRAME_H
