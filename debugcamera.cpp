#include "debugcamera.h"

DebugCamera::DebugCamera(
        const std::string fname,
        int frWidth, int frHeight,
        int dataHeight
) : CameraModel(),
    ifname(fname),
    framesize(frWidth * dataHeight * sizeof(uint16_t))
{
    frame_width = frWidth;
    frame_height = frHeight;
    data_height = dataHeight;
    camera_type = DEFAULT;
}

DebugCamera::~DebugCamera()
{
}

bool DebugCamera::start()
{
    dev_p.open(ifname, std::ios::in | std::ios::binary);
    if (!dev_p.is_open()) {
        qDebug("Could not open file. Does it exist?");
        dev_p.clear();
        return false;
    }

    dev_p.unsetf(std::ios::skipws);
    dev_p.seekg(0, std::ios::end);
    bufsize = dev_p.tellg();
    dev_p.seekg(0, std::ios::beg);
    nFrames = bufsize / framesize;
    qDebug() << ifname.data() << " has a total size of " << bufsize << " bytes, which corresponds to " << nFrames <<" frames.";
    frame_buf.reserve(nFrames);
    for (int n = 0; n < nFrames; n++) {
        dev_p.read(reinterpret_cast<char*>(frame_buf[n].data()), framesize);
    }
    curIndex = -1;
    dev_p.close();

    return true;
}

uint16_t* DebugCamera::getFrame()
{
    curIndex++;
    if (curIndex > nFrames) {
        curIndex = 0;
    }
    return frame_buf[curIndex].data();
}
