#ifndef LVFRAME_H
#define LVFRAME_H

#include <errno.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <QtGlobal>
#include <QDebug>

#include "constants.h"

struct LVFrame
{
    uint16_t* raw_data;
    float* dsf_data;
    float* sdv_data;
    uint32_t* hist_data;
    const unsigned int frSize;

    LVFrame(const unsigned int frame_size) : frSize(frame_size)
    {
        try {
            raw_data = new uint16_t[frame_size];
            dsf_data = new float[frame_size];
            sdv_data = new float[frame_size];
            hist_data = new uint32_t[NUMBER_OF_BINS];
        } catch (std::bad_alloc&) {
            qFatal("Not enough memory to allocate frame buffer.");
            throw;
        }
        rlimit cur_lims;
        checkError(getrlimit(RLIMIT_MEMLOCK, &cur_lims));
        rlimit new_lims = { RLIM_INFINITY, RLIM_INFINITY };
        checkError(setrlimit(RLIMIT_MEMLOCK, &new_lims));
        checkError(mlock(raw_data, frSize * sizeof(uint16_t)));
        checkError(mlock(sdv_data, frSize * sizeof(float)));
    }
    ~LVFrame()
    {
        checkError(munlock(raw_data, frSize * sizeof(uint16_t)));
        checkError(munlock(sdv_data, frSize * sizeof(float)));
        delete raw_data;
        delete dsf_data;
        delete sdv_data;
        delete hist_data;
    }

    void checkError(int error)
    {
        char buffer[256];
        if (error == -1) {
#if (__APPLE__ && __MACH__)
            int res;
            res = strerror_r(errno, buffer, 256);
            qWarning("[Errno %d]: %s: %d", errno, buffer, res);
#else
            char* err;
            err = strerror_r(errno, err, 256);
            qWarning("[Errno %d]: %s", errno, err);
#endif
        }
    }
};

#endif // LVFRAME_H
