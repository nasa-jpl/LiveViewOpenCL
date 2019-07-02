#ifndef EDTCAMERA_H
#define EDTCAMERA_H


#include <cstring>
#include <mutex>

#include <QtGlobal>

#include "image_type.h"
#include "cameramodel.h"
#include "EDT_include/edtinc.h"

class CLCamera : public CameraModel
{
public:
    CLCamera(int channel_num = 0, int num_buffers = 64, int filter_refresh_rate = 10);
    virtual ~CLCamera();

    virtual bool start();
    virtual uint16_t* getFrame();
    virtual bool isRunning();

private:
   PdvDev* dev_p;
    uint16_t* image_p;
    int channel;
    int numbufs;
    int frate;
    int overrun, overruns;
    int timeouts, last_timeouts;
    bool recovering_timeout;
    std::mutex dev_p_lock;
};

#endif // EDTCAMERA_H
