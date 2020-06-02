#ifndef LVFRAME_H
#define LVFRAME_H

#include <errno.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/resource.h>

#include <QtGlobal>
#include <QDebug>

#include "constants.h"

struct LVFrame
{
    uint16_t *raw_data;
    float *dsf_data;
    float *sdv_data;
    float *snr_data;
    uint32_t *hist_data;
    float *spectral_mean;
    float *spatial_mean;
    float *frame_fft;
    const int frSize;

    LVFrame(const int frame_width, const int frame_height) : frSize(frame_width * frame_height)
    {
        try {
            raw_data = new uint16_t[frSize];
            dsf_data = new float[frSize];
            sdv_data = new float[frSize];
            snr_data = new float[frSize];
            hist_data = new uint32_t[NUMBER_OF_BINS];
            spectral_mean = new float[frame_height];
            spatial_mean = new float[frame_width];
            frame_fft = new float[MAX_FFT_SIZE];
        } catch (std::bad_alloc&) {
            qFatal("Not enough memory to allocate frame buffer.");
        }
        rlimit cur_lims;
        checkError(getrlimit(RLIMIT_MEMLOCK, &cur_lims));
        if (cur_lims.rlim_cur != RLIM_INFINITY) {
            rlimit new_limit = { RLIM_INFINITY, RLIM_INFINITY };
            checkError(setrlimit(RLIMIT_MEMLOCK, &new_limit));
        }
 //       checkError(mlock(raw_data, frSize * sizeof(uint16_t)));
        checkError(mlock(sdv_data, size_t(frSize) * sizeof(float)));
    }
    ~LVFrame()
    {
//        checkError(munlock(raw_data, frSize * sizeof(uint16_t)));
        checkError(munlock(sdv_data, size_t(frSize) * sizeof(float)));
        delete dsf_data;
        delete sdv_data;
        delete snr_data;
        delete hist_data;
        delete spectral_mean;
        delete spatial_mean;
        delete frame_fft;
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
        err = strerror_r(errno, buffer, 256);
        qWarning("[Errno %d]: %s", errno, err);
#endif
        }
    }
};

#endif // LVFRAME_H
