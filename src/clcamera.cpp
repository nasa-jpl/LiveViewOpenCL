#include "clcamera.h"
#ifndef EDTCAMERA_H

CLCamera::CLCamera(int channel_num,
                   int num_buffers,
                   int filter_refresh_rate) :
    CameraModel(), channel(channel_num), numbufs(num_buffers),
    frate(filter_refresh_rate), overrun(0), overruns(0),
    timeouts(0), last_timeouts(0), recovering_timeout(false)
{
    source_type = CAMERA_LINK;
}

CLCamera::~CLCamera()
{
    std::lock_guard<std::mutex> lock{dev_p_lock};
    int dummy_frame;
    // collect the last frame to avoid a core dump
    pdv_wait_last_image(dev_p, &dummy_frame);
    pdv_close(dev_p);
}

bool CLCamera::start()
{
    std::lock_guard<std::mutex> lock{dev_p_lock};
    dev_p = nullptr;
    dev_p = pdv_open_channel(EDT_INTERFACE, 0, channel);
    if (dev_p == nullptr) {
        qFatal("Could not open Camera Link device on channel 0. Is there a camera connected and powered on?");
    }
    pdv_flush_fifo(dev_p);
    int size = pdv_get_dmasize(dev_p);
    camera_name = pdv_get_cameratype(dev_p);
    qDebug() << "Hardware Type (as reported by driver): " << camera_name;

    frame_width = pdv_get_width(dev_p);
    data_height = pdv_get_height(dev_p);

    // perform slightly different hardware interface tasks depending of the type of camera
    switch (size) {
        case 481*640*sizeof(uint16_t): camera_type = CL_6604A; break;
        case 285*640*sizeof(uint16_t): camera_type = CL_6604A; break; // legacy FPA geometry
        default: camera_type = CL_6604B; break; // Typically 1280x480 Chroma geometry
    }

    frame_height = camera_type == CL_6604A ? data_height - 1 : data_height;

    pdv_multibuf(dev_p, numbufs);
    numbufs = 16;
    pdv_start_images(dev_p, numbufs);
    pdv_set_timeout(dev_p, 2000);

    return true;
}

bool CLCamera::isRunning()
{
    return !(bool)recovering_timeout;
}

uint16_t* CLCamera::getFrame()
{
    std::lock_guard<std::mutex> lock{dev_p_lock};
    pdv_start_image(dev_p);
    image_p = (uint16_t*)pdv_wait_image(dev_p);

    if ((overrun = (edt_reg_read(dev_p, PDV_STAT) & PDV_OVERRUN)))
        ++overruns;

    timeouts = pdv_timeouts(dev_p);
    if (timeouts > last_timeouts) {
        pdv_timeout_restart(dev_p, true);
        last_timeouts = timeouts;
        recovering_timeout = true;
        emit timeout();
    } else if (recovering_timeout) {
        pdv_timeout_restart(dev_p, true);
        recovering_timeout = false;
    }

    return image_p;

}

#endif
