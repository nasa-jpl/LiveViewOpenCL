#include "ssdcamera.h"

SSDCamera::SSDCamera(const std::string search_dir,
        int frWidth, int frHeight,
        int dataHeight
) : CameraModel(),
    data_dir(search_dir),
    framesize(frWidth * dataHeight * sizeof(uint16_t)),
    headsize(frWidth * sizeof(uint16_t))
{
    frame_width = frWidth;
    frame_height = frHeight;
    data_height = dataHeight;
    camera_type = ITB;
}

SSDCamera::~SSDCamera()
{
}

bool SSDCamera::start()
{
    return true;
}

void SSDCamera::setDir(const char* dirname)
{
    data_dir = dirname;
    read_files.clear();
    xio_files.clear();
    dev_p.clear();
    dev_p.close();
    curIndex = -1;
    readFile();
}

std::string SSDCamera::getFname()
{
    std::string fname; // will return empty string if no unread files are found.
    std::vector<std::string> fname_list;
    bool has_file = false;
    bool has_read = false;
    os::listdir(fname_list, data_dir);
    for (auto f = fname_list.begin(); f != fname_list.end(); ++f) {
        has_file = std::find(xio_files.begin(), xio_files.end(), *f) != xio_files.end();
        if ((*f).empty() or os::getext(*f) != "xio" or has_file)
            continue;

        xio_files.push_back(*f);
    }
    /* if necessary, there may need to be code to sort the "frames" in the data directory
    * by product name, as mtime is unreliable.
    * std::sort(xio_files.begin(), xio_files.end(),
    *          [] (std::string const &a, std::string const &b) { return a.data() < b.data(); });
    */
    for (auto i = xio_files.begin(); i != xio_files.end(); ++i) {
        has_read = std::find(read_files.begin(), read_files.end(), *i) != read_files.end();
        if (!has_read) {
            fname = *i;
            break;
        }
    }

    return fname;
}

void SSDCamera::readFile()
{
    ifname = getFname();
    if (ifname.empty()) {
        if (dev_p.is_open())
            dev_p.close();
        nFrames = TIMEOUT_DURATION;
        frame_buf.reserve(nFrames);
        for (int n = 0; n < nFrames; ++n)
            std::fill(frame_buf[n].begin(), frame_buf[n].end(), 0);
    } else {
        std::vector<unsigned char> header(headsize);
        read_files.push_back(ifname);
        dev_p.open(ifname, std::ios::in | std::ios::binary);
        if (!dev_p.is_open()) {
            qDebug() << "Could not open file " << ifname.data() << ". Does it exist?";
            dev_p.clear();
            readFile();
            return;
        }
        qDebug() << "Successfully opened " << ifname.data();
        dev_p.unsetf(std::ios::skipws);
        dev_p.seekg(0, std::ios::end);
        int filesize = dev_p.tellg();
        nFrames = (filesize - headsize) / framesize;
        dev_p.seekg(0, std::ios::beg);
        // qDebug() << "File size is " << filesize - headsize <<
        //            " bytes, which corresponds to" << nFrames << "frames.";

        dev_p.read(reinterpret_cast<char*>(header.data()), headsize);
        frame_buf.reserve(nFrames);
        for (int n = 0; n < nFrames; ++n) {
            dev_p.read(reinterpret_cast<char*>(frame_buf[n].data()), framesize);
        }
        frame_valid = true;
        dev_p.close();
    }
}

uint16_t* SSDCamera::getFrame()
{
    curIndex++;
    if (curIndex >= nFrames) {
        readFile();
        curIndex = 0;
    }
    if (frame_valid) {
        return frame_buf[curIndex].data();
    } else {
        return dummy.data();
    }
}
