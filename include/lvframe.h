#ifndef LVFRAME_H
#define LVFRAME_H

#include <errno.h>
#include <sys/mman.h>

#include <QtGlobal>
#include <QDebug>

#include "constants.h"

struct LVFrame
{
    uint16_t* raw_data;
    float* dsf_data;
    float* sdv_data;
    const unsigned int frSize;

    LVFrame(const unsigned int frame_size) : frSize(frame_size)
    {
        int lock_err;
        try {
            raw_data = new uint16_t[frame_size];
            dsf_data = new float[frame_size];
            sdv_data = new float[frame_size];
        } catch (std::bad_alloc&) {
            qFatal("Not enough memory to allocate frame buffer.");
            throw;
        }
        lock_err = mlock(raw_data, frame_size * sizeof(uint16_t));
        if (lock_err == -1) {
            char buffer[256];
            strerror_r(errno, buffer, 256);
            qDebug() << buffer;
            qWarning("Warning: Unable to lock memory to physical address space.");
            qWarning("Perfomance will decrease, and undefined behavior may occur.");
        }
        lock_err = mlock(sdv_data, frame_size * sizeof(float));
        if (lock_err == -1) {
            char buffer[256];
            strerror_r(errno, buffer, 256);
            qDebug() << buffer;
            qWarning("Warning: Unable to lock memory to physical address space.");
            qWarning("Perfomance will decrease, and undefined behavior may occur.");
        }
    }
    ~LVFrame()
    {
        int unlock_err;
        unlock_err = munlock(raw_data, frSize);
        if (unlock_err == -1) {
            char buffer[256];
            strerror_r(errno, buffer, 256);
            qDebug() << buffer;
        }
        unlock_err = munlock(sdv_data, frSize);
        if (unlock_err == -1) {
            char buffer[256];
            strerror_r(errno, buffer, 256);
            qDebug() << buffer;
        }
        delete raw_data;
        delete dsf_data;
        delete sdv_data;
    }
};

#endif // LVFRAME_H
