#include "envicamera.h"

ENVICamera::ENVICamera(QObject *parent) :
    CameraModel(parent), framesize(640 * 480),
    curIndex(0), nFrames(32)
{
    // frame_width = frWidth;
    // frame_height = frHeight;
    // data_height = dataHeight;
    camera_type = DEFAULT;
    source_type = DEBUG;
}

bool ENVICamera::start()
{
    dev_p.open(ifname, std::ios::in | std::ios::binary);
    if (!dev_p.is_open()) {
        qDebug("Could not open file. Does it exist?");
        dev_p.clear();
        return running.load();
    }

    qDebug() << "Successfully opened" << ifname.data();
    dev_p.unsetf(std::ios::skipws);

    dev_p.seekg(0, std::ios::end);
    bufsize = dev_p.tellg();
    dev_p.seekg(0, std::ios::beg);

    nFrames = bufsize / framesize;
    qDebug() << ifname.data() << " has a total size of " << bufsize << " bytes, which corresponds to " << nFrames <<" frames.";

    frame_buf.reserve(nFrames);
    for (unsigned int n = 0; n < nFrames; n++) {
        dev_p.read(reinterpret_cast<char*>(frame_buf[n].data()), framesize);
    }
    curIndex = -1;
    dev_p.close();
    running.store(true);
    return running.load();
}

uint16_t* ENVICamera::getFrame()
{
    curIndex++;
    if (curIndex > nFrames) {
        curIndex = 0;
    }
    return frame_buf[curIndex].data();
}
