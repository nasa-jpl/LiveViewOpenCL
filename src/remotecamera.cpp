#include "remotecamera.h"

RemoteCamera::RemoteCamera(int frWidth,
        int frHeight, int dataHeight, int Descriptor,
        QObject *parent
) : CameraModel(parent), nFrames(32), framesize(0),
    headsize(frWidth * int(sizeof(uint16_t))), image_no(0),
    tmoutPeriod(100) // milliseconds
{
    source_type = RC;
    camera_type = ETH_RC;
    frame_width = frWidth;
    frame_height = frHeight;
    data_height = dataHeight;

    socket = new QTcpSocket();
    socket->setSocketDescriptor(Descriptor);
    qDebug() << socket->readAll(); // I need to do this to make sure I don't miss anything (according to sources :))

    connect(socket, &QTcpSocket::stateChanged, this, &RemoteCamera::SocketStateChanged);
    connect(socket, &QTcpSocket::readyRead, this, &RemoteCamera::SocketReady);
    socket->waitForConnected();
    qDebug() << "Waiting for connected" << socket->state(); // This line is required to check that we are still connected.
//    socket->write("tHiS iS a BUnCh Of DaTA");
//    qDebug() << "Wrote";
//    socket->waitForBytesWritten();
//    qDebug() << "Waited for written";
//    socket->waitForReadyRead();
//    qDebug() << "Waited for read";

//    // Convert the data
//    QByteArray buffer = socket->readAll();


    dummy.resize(size_t(frame_width * data_height));
    std::fill(dummy.begin(), dummy.end(), 0);

    temp_frame.resize(size_t(frame_width * data_height));
    std::fill(temp_frame.begin(), temp_frame.end(), 0);
    running.store(true);
}

RemoteCamera::~RemoteCamera()
{
    running.store(false);
    socket->disconnect();
    emit timeout();
    is_reading = false;
    readLoopFuture.waitForFinished();
}

//std::string RemoteCamera::getFname()
//{
//    std::string fname; // will return empty string if no unread files are found.
//    std::vector<std::string> fname_list;
//    bool has_file = false;
//    if (data_dir.empty()) {
//        return fname;
//    }
//    if (image_no < xio_files.size()) {
//        fname = xio_files[image_no++];
//    } else {
//        os::listdir(fname_list, data_dir);
//        if (fname_list.size() < 1) {
//            return fname;
//        }
//        /* if necessary, there may need to be code to sort the "frames" in the data directory
//        * by product name, as mtime is unreliable.
//        */
//        std::sort(fname_list.begin(), fname_list.end(), doj::alphanum_less<std::string>());
//        for (auto f = fname_list.end() - 1; f != fname_list.begin(); --f) {
//            has_file = std::find(xio_files.begin(), xio_files.end(), *f) != xio_files.end();
//            std::string ext = os::getext(*f);
//            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
//            if ((*f).empty() or (std::strcmp(ext.data(), "xio") != 0 and
//                                 std::strcmp(ext.data(), "decomp") != 0)) {
//                continue;
//            } else if (has_file) {
//                break;
//            } else {
//                xio_files.emplace_back(*f);
//            }

//        }

//        if (image_no < xio_files.size()) {
//            fname = xio_files[image_no++];
//        }
//    }

//    return fname;
//}

//void RemoteCamera::readFile()
//{
//    is_reading = true;
//    bool validFile = false;
//    while(!validFile) {
//        ifname = getFname();
//        if (ifname.empty()) {
//            if (dev_p.is_open()) {
//                dev_p.close();
//            }

//            if (running.load()) {
//                running.store(false);
//                emit timeout();
//            }
//            return; //If we're out of files, give up
//        }
//        // otherwise check if data is valid
//        dev_p.open(ifname, std::ios::in | std::ios::binary);
//        if (!dev_p.is_open()) {
//            qDebug() << "Could not open file" << ifname.data() << ". Does it exist?";
//            dev_p.clear();
//            readFile();
//            return;
//        }

//        // qDebug() << "Successfully opened " << ifname.data();
//        dev_p.unsetf(std::ios::skipws);

//        dev_p.read(reinterpret_cast<char*>(header.data()), headsize);

//        std::streampos filesize(0);
//        std::string ext = os::getext(ifname);
//        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
//        if (!std::strcmp(ext.data(), "decomp")) {
//            dev_p.seekg(0, std::ios::end);
//            filesize = dev_p.tellg();
//            dev_p.seekg(headsize, std::ios::beg);
//        } else {
//            // convert the raw hex string to decimal, one digit at a time.
//            filesize = int(header[7]) * 16777216 + int(header[6]) * 65536 + int(header[5]) * 256 + int(header[4]);
//        }

//        framesize = static_cast<size_t>(filesize / nFrames);
//        if (framesize == 0) { //If header reports a 0 filesize (invalid data), then skip this file.
//            dev_p.close();
//            qDebug().nospace() << "Skipped file \"" << ifname.data() << "\" due to invalid data.";
//        } else { //otherwise we load it
//            validFile = true;
//            dev_p.seekg(headsize, std::ios::beg);

//            // qDebug() << "File size is" << filesize << "bytes, which corresponds to a framesize of" << framesize << "bytes.";

//            std::vector<uint16_t> zero_vec(size_t(frame_width * data_height) - (size_t(framesize) / sizeof(uint16_t)));
//            std::fill(zero_vec.begin(), zero_vec.end(), 0);

//            std::vector<uint16_t> copy_vec(size_t(framesize), 0);

//            for (int n = 0; n < nFrames; ++n) {
//                dev_p.read(reinterpret_cast<char*>(copy_vec.data()), std::streamsize(framesize));
//                frame_buf.emplace_front(copy_vec);

//                if (framesize / sizeof(uint16_t) < size_t(frame_width * data_height)) {
//                    std::copy(zero_vec.begin(), zero_vec.end(), frame_buf[size_t(n)].begin() + framesize / sizeof(uint16_t));
//                }
//            }

//            running.store(true);
//            emit started();
//            dev_p.close();
//        }
//    }
//}

//void RemoteCamera::readLoop()
//{
//    QTime remTime;
//    do {
//        // Yeah yeah whatever it's a magic buffer size recommendation
//        if (frame_buf.size() <= 96) {
//            readFile();
//        } else {
//            remTime = QTime::currentTime().addMSecs(tmoutPeriod);
//            while(QTime::currentTime() < remTime) {
//                QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
//            }
//        }
//    } while (is_reading);
//}

uint16_t* RemoteCamera::getFrame()
{
    // Prompt the server to send a frame over
    if(is_reading && socket->isWritable()) // Validate that socket is ready
    {
        if(socket->isWritable() && !is_receiving) // Validate that socket is ready
        {
            is_receiving = true; // Forces only one request to go out at a time
            //qDebug() << "Getting frame from socket...";
            socket->write("Ready");
            //qDebug() << "Wrote";
            socket->waitForBytesWritten(100);
            //qDebug() << "Waited for written";
            if (!socket->waitForReadyRead(500)) // If it timed out
            {
                qDebug() << "Timed out";
                return temp_frame.data(); // Return existing frame
            }
            //qDebug() << "Waited for read";

            // Convert the data
            QByteArray buffer = socket->read(framesize*2);
            QDataStream dstream(buffer);
            size_t dataSize = buffer.size();

            for (size_t i = 0; i < dataSize; i++) { dstream >> temp_frame[i]; } // Do I need a for loop?
            //qDebug() << "Returning Data";
            is_receiving = false;
            return temp_frame.data();
        } else {
            //qDebug() << "Dummy data";
            //qDebug() << "---";
            return temp_frame.data();
        }
    } else {
        return dummy.data();
    }
}

void RemoteCamera::SocketStateChanged(QTcpSocket::SocketState state)
{
    qDebug() << "Socket has changed state" << state;
    switch (state) {
    case QTcpSocket::ConnectedState:
        // Do nothing, we should be in this state no but investigate becuase we would need to be somewhere else
        break;
    case QTcpSocket::UnconnectedState:
        is_reading = false;
        emit timeout();
        break;
    default:
        qDebug() << "WEIRD STATE ENCOUNTERED. QUITTING";
        emit timeout();
        break;
    }
}
