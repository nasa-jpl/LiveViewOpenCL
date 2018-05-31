#include "ssdcamera.h"

SSDCamera::SSDCamera(unsigned int frWidth,
        unsigned int frHeight, unsigned int dataHeight,
        QObject *parent
) : CameraModel(parent),
    nFrames(32),
    headsize(frWidth * sizeof(uint16_t)),
    image_no(0)
{
    frame_width = frWidth;
    frame_height = frHeight;
    data_height = dataHeight;

    header.reserve(headsize);
    std::fill(header.begin(), header.end(), 0);

    dummy.reserve(frame_width * data_height);
    std::fill(dummy.begin(), dummy.end(), 0);
    for (unsigned int n = 0; n < nFrames; n++) {
        frame_buf.push_back(std::vector<uint16_t>(frame_width * data_height, 0));
    }

    camera_type = ITB;
}

SSDCamera::~SSDCamera()
{
}

bool SSDCamera::start()
{
    return true;
}

void SSDCamera::setDir(const char *dirname)
{
    data_dir = dirname;
    qDebug() << data_dir.data();
    if (data_dir.empty()) {
        if (running.load()) {
            running.store(false);
            emit timeout();
        }
        return;
    }
    xio_files.clear();
    dev_p.clear();
    dev_p.close();
    curIndex.store(-1);
    image_no = 0;
    std::vector<std::string> fname_list;
    os::listdir(fname_list, data_dir);

    // Sort the frames in the product directory by filename, as mtime is unreliable.
    std::sort(fname_list.begin(), fname_list.end(), doj::alphanum_less<std::string>());

    for (auto f = fname_list.begin(); f != fname_list.end(); ++f) {
        if (f->empty() or os::getext(*f) != "xio")
            continue;

        xio_files.push_back(*f);
    }
    readFile();
}

std::string SSDCamera::getFname()
{
    std::string fname; // will return empty string if no unread files are found.
    std::vector<std::string> fname_list;
    bool has_file = false;
    if (data_dir.empty()) {
        return fname;
    }
    if (image_no < xio_files.size()) {
        fname = xio_files[image_no++];
    } else {
        os::listdir(fname_list, data_dir);
        /* if necessary, there may need to be code to sort the "frames" in the data directory
        * by product name, as mtime is unreliable.
        */
        // std::sort(fname_list.begin(), fname_list.end(), doj::alphanum_less<std::string>());
        for (auto f = fname_list.end() - 1; f != fname_list.begin(); --f) {
            has_file = std::find(xio_files.begin(), xio_files.end(), *f) != xio_files.end();
            if ((*f).empty() or os::getext(*f) != "xio")
                continue;
            else if (has_file) {
                break;
            }

            xio_files.push_back(*f);
        }

        if (image_no < xio_files.size()) {
            fname = xio_files[image_no++];
        }
    }

    return fname;
}

void SSDCamera::readFile()
{
    ifname = getFname();
    if (ifname.empty()) {
        if (dev_p.is_open()) {
            dev_p.close();
        }

        if (running.load()) {
            running.store(false);
            emit timeout();
        }
    } else {
        dev_p.open(ifname, std::ios::in | std::ios::binary);
        if (!dev_p.is_open()) {
            qDebug() << "Could not open file " << ifname.data() << ". Does it exist?";
            dev_p.clear();
            readFile();
            return;
        }

        // qDebug() << "Successfully opened " << ifname.data();
        dev_p.unsetf(std::ios::skipws);

        dev_p.read(reinterpret_cast<char*>(header.data()), headsize);

        // convert the raw hex string to decimal, one digit at a time.
        int filesize = int(header[7]) * 16777216 + int(header[6]) * 65536 + int(header[5]) * 256 + int(header[4]);

        framesize = filesize / int(nFrames);
        dev_p.seekg(headsize, std::ios::beg);

        // qDebug() << "File size is" << filesize << "bytes, which corresponds to a framesize of" << framesize << "bytes.";

        for (unsigned int n = 0; n < nFrames; ++n) {
            dev_p.read(reinterpret_cast<char*>(frame_buf[n].data()), framesize);
        }

        running.store(true);
        dev_p.close();
    }
}

uint16_t* SSDCamera::getFrame()
{
    curIndex++;

    if (curIndex.load() >= nFrames) {
        readFile();
        curIndex.store(0);
    }

    if (running.load()) {
        return frame_buf[curIndex.load()].data();
    } else {
        return dummy.data();
    }
}
