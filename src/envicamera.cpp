#include "envicamera.h"

ENVICamera::ENVICamera(int frWidth,
                       int frHeight,
                       int dataHeight,
                       QObject *parent) :
    CameraModel(parent), framesize(frWidth * dataHeight),
    curIndex(0), nFrames(0), chunkFrames(32), framesRead(0),
    tmoutPeriod(100) // milliseconds
{
    frame_width = frWidth;
    frame_height = frHeight;
    data_height = dataHeight;
    camera_type = SSD_ENVI;
    source_type = ENVI;

    dummy.resize(size_t(framesize));
    std::fill(dummy.begin(), dummy.end(), 0);
    for (int n = 0; n < nFrames; n++) {
        frame_buf.emplace_back(std::vector<uint16_t>(size_t(frame_width * data_height), 0));
    }
}

bool ENVICamera::start()
{
    running.store(true);
    return running.load();
}

bool ENVICamera::readHeader(std::string hdr_fname)
{
    std::ifstream infile(hdr_fname);
    if (!infile.is_open()) {
        qDebug("Unable to read ENVI header file!");
        return false;
    }
    std::string line, item;
    std::vector< std::vector<std::string> > TextData;
    HDRData.lines = 0;
    HDRData.samples = 0;
    HDRData.bands = 0;
    HDRData.nbits = 0;
    HDRData.interleave = fwBIL;

    for (std::string line; std::getline(infile, line); ) {
        std::istringstream iss(line);
        std::vector<std::string> lineData;
        while (std::getline(iss, item, '=')) {
            lineData.push_back(os::trim(item));
        }
        TextData.push_back(lineData);
        if (lineData[0].compare("samples") == 0) {
            HDRData.samples = std::stoi(lineData[1]);
        } else if (lineData[0].compare("lines") == 0) {
            HDRData.lines = std::stoi(lineData[1]);
        } else if (lineData[0].compare("bands") == 0) {
            HDRData.bands = std::stoi(lineData[1]);
        } else if (lineData[0].compare("interleave") == 0) {
            HDRData.interleave = fwBIL;
            if (lineData[1].compare("bip") == 0) {
                HDRData.interleave = fwBIP;
            } else if (lineData[1].compare("bsq") == 0) {
                HDRData.interleave = fwBSQ;
            }
        } else if (lineData[0].compare("data type") == 0) {
            HDRData.nbits = std::stoi(lineData[1]);
        }
    }

    nFrames = HDRData.lines;
    if (HDRData.samples != frame_width || HDRData.bands != frame_width) {
        qDebug("Frame geometry of input ENVI file does not match specified geometry.");
        qDebug() << "Please restart LiveView with a geometry of" << HDRData.samples << "by" << HDRData.bands;
        return false;
    } else if (HDRData.interleave != fwBIL) {
        // fite meeeee ヽ(´ー｀)┌
        qDebug("Only BIL bit organization is currently supported. Please bother Jackie to make a decoder.");
        return false;
    }
    return true;
}

bool ENVICamera::readFile(std::string fname)
{
    is_reading = false;
    readLoopFuture.waitForFinished();
    if (dev_p.is_open()) {
        dev_p.clear();
    }

    ifname = fname;

    // Guess the ENVI header name based on file extension replacement
    std::string hdr_fname;
    if (ifname.find_last_of('.') != std::string::npos) {
        hdr_fname = ifname.substr(0, fname.find_last_of('.') + 1) + "hdr";
    } else {
        hdr_fname = ifname + ".hdr";
    }
    // Now check if we guessed correctly
    struct stat buffer;
    bool read_head = false;
    if (stat(hdr_fname.c_str(), &buffer) == 0) {
        read_head = readHeader(hdr_fname);
        if (!read_head) {
            return false;
        }
    } else {
        // An ENVI file without a header is like a yee without a haw, we must report that the file is unreadable
        qDebug("Could not find associated header. Please check there is a detached ENVI header associated with the data.");
        return false;
    }

    dev_p.open(ifname, std::ios::in | std::ios::binary);
    if (!dev_p.is_open()) {
        qDebug("Could not open file. Does it exist?");
        dev_p.clear();
        return false;
    }

    readLoopFuture = QtConcurrent::run(this, &ENVICamera::readLoop);

    return true;
}

void ENVICamera::readLoop()
{
    QTime remTime;
    do {
        // Yeah yeah whatever it's a magic number/buffer size recommendation
        if (frame_buf.size() <= 96) {
            if (framesRead >= nFrames) {
                // drops out of the loop => 再見
                is_reading = false;
                continue;
            }
            int nextFrames = framesRead + chunkFrames > nFrames ? nFrames - framesRead : chunkFrames;
            frame_buf.reserve(size_t(nextFrames));
            for (int n = 0; n < nextFrames; n++) {
                dev_p.read(reinterpret_cast<char*>(frame_buf[size_t(n)].data()), framesize);
            }
        } else {
            remTime = QTime::currentTime().addMSecs(tmoutPeriod);
            while(QTime::currentTime() < remTime) {
                QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
            }
        }
    } while (is_reading);
}

uint16_t* ENVICamera::getFrame()
{
    if (!frame_buf.empty() && is_reading) {
        temp_frame = frame_buf.back();
        frame_buf.pop_back();
        return temp_frame.data();
    } else {
        return dummy.data();
    }
}
