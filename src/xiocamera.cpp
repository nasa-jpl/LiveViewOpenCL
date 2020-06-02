#include "xiocamera.h"

XIOCamera::XIOCamera(int frWidth,
        int frHeight, int dataHeight,
        QObject *parent
) : CameraModel(parent), nFrames(32), framesize(0),
    headsize(frWidth * int(sizeof(uint16_t))), image_no(0),
    tmoutPeriod(100) // milliseconds
{
    source_type = XIO;
    camera_type = SSD_XIO;
    frame_width = frWidth;
    frame_height = frHeight;
    data_height = dataHeight;

    header.resize(size_t(headsize));
    std::fill(header.begin(), header.end(), 0);

    dummy.resize(size_t(frame_width * data_height));
    std::fill(dummy.begin(), dummy.end(), 0);
    for (int n = 0; n < nFrames; n++) {
        frame_buf.emplace_back(std::vector<uint16_t>(size_t(frame_width * data_height), 0));
    }
}

XIOCamera::~XIOCamera()
{
    running.store(false);
    emit timeout();
    is_reading = false;
    readLoopFuture.waitForFinished();
}

void XIOCamera::setDir(const char *dirname)
{
    is_reading = false;

    while (!frame_buf.empty()) {
        frame_buf.pop_back();
    }

    if (readLoopFuture.isRunning()) {
        readLoopFuture.waitForFinished();
    }
    data_dir = dirname;
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
    image_no = 0;
    std::vector<std::string> fname_list;
    os::listdir(fname_list, data_dir);

    // Sort the frames in the product directory by filename, as mtime is unreliable.
    std::sort(fname_list.begin(), fname_list.end(), doj::alphanum_less<std::string>());

    for (auto &f : fname_list) {
        std::string ext = os::getext(f);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (f.empty() or std::strcmp(ext.data(), "xio") != 0 or std::strcmp(ext.data(), "decomp") != 0)
            continue;

        xio_files.emplace_back(f);
    }

    running.store(true);
    emit started();

    readLoopFuture = QtConcurrent::run(this, &XIOCamera::readLoop);
}

std::string XIOCamera::getFname()
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
        if (fname_list.size() < 1) {
            return fname;
        }
        /* if necessary, there may need to be code to sort the "frames" in the data directory
        * by product name, as mtime is unreliable.
        */
        std::sort(fname_list.begin(), fname_list.end(), doj::alphanum_less<std::string>());
        for (auto f = fname_list.end() - 1; f != fname_list.begin(); --f) {
            has_file = std::find(xio_files.begin(), xio_files.end(), *f) != xio_files.end();
            std::string ext = os::getext(*f);
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if ((*f).empty() or (std::strcmp(ext.data(), "xio") != 0 and
                                 std::strcmp(ext.data(), "decomp") != 0)) {
                continue;
            } else if (has_file) {
                break;
            } else {
                xio_files.emplace_back(*f);
            }

        }

        if (image_no < xio_files.size()) {
            fname = xio_files[image_no++];
        }
    }

    return fname;
}

void XIOCamera::readFile()
{
    is_reading = true;
    bool validFile = false;
    while(!validFile) {
        ifname = getFname();
        if (ifname.empty()) {
            if (dev_p.is_open()) {
                dev_p.close();
            }

            if (running.load()) {
                running.store(false);
                emit timeout();
            }
            return; //If we're out of files, give up
        }
        // otherwise check if data is valid
        dev_p.open(ifname, std::ios::in | std::ios::binary);
        if (!dev_p.is_open()) {
            qDebug() << "Could not open file" << ifname.data() << ". Does it exist?";
            dev_p.clear();
            readFile();
            return;
        }

        // qDebug() << "Successfully opened " << ifname.data();
        dev_p.unsetf(std::ios::skipws);

        dev_p.read(reinterpret_cast<char*>(header.data()), headsize);

        std::streampos filesize(0);
        std::string ext = os::getext(ifname);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (!std::strcmp(ext.data(), "decomp")) {
            dev_p.seekg(0, std::ios::end);
            filesize = dev_p.tellg();
            dev_p.seekg(headsize, std::ios::beg);
        } else {
            // convert the raw hex string to decimal, one digit at a time.
            filesize = int(header[7]) * 16777216 + int(header[6]) * 65536 + int(header[5]) * 256 + int(header[4]);
        }

        framesize = static_cast<size_t>(filesize / nFrames);
        if (framesize == 0) { //If header reports a 0 filesize (invalid data), then skip this file.
            dev_p.close();
            qDebug().nospace() << "Skipped file \"" << ifname.data() << "\" due to invalid data.";
        } else { //otherwise we load it
            validFile = true;
            dev_p.seekg(headsize, std::ios::beg);

            // qDebug() << "File size is" << filesize << "bytes, which corresponds to a framesize of" << framesize << "bytes.";

            std::vector<uint16_t> zero_vec(size_t(frame_width * data_height) - (size_t(framesize) / sizeof(uint16_t)));
            std::fill(zero_vec.begin(), zero_vec.end(), 0);

            std::vector<uint16_t> copy_vec(size_t(framesize), 0);

            for (int n = 0; n < nFrames; ++n) {
                dev_p.read(reinterpret_cast<char*>(copy_vec.data()), std::streamsize(framesize));
                frame_buf.emplace_front(copy_vec);

                if (framesize / sizeof(uint16_t) < size_t(frame_width * data_height)) {
                    std::copy(zero_vec.begin(), zero_vec.end(), frame_buf[size_t(n)].begin() + framesize / sizeof(uint16_t));
                }
            }

            running.store(true);
            emit started();
            dev_p.close();
        }
    }
}

void XIOCamera::readLoop()
{
    QTime remTime;
    do {
        // Yeah yeah whatever it's a magic buffer size recommendation
        if (frame_buf.size() <= 96) {
            readFile();
        } else {
            remTime = QTime::currentTime().addMSecs(tmoutPeriod);
            while(QTime::currentTime() < remTime) {
                QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
            }
        }
    } while (is_reading);
}

uint16_t* XIOCamera::getFrame()
{
    if (!frame_buf.empty() && is_reading) {
        temp_frame = frame_buf.back();
        frame_buf.pop_back();
        return temp_frame.data();
    } else {
        return dummy.data();
    }
}
