#include "remotecamera.h"

RemoteCamera::RemoteCamera(int frWidth,
        int frHeight, int dataHeight, int Descriptor,
        QObject *parent
) : CameraModel(parent), nFrames(32), framesize(frWidth * dataHeight),
    headsize(frWidth * int(sizeof(uint16_t))), image_no(0),
    tmoutPeriod(100) // milliseconds
{
    source_type = RC;
    camera_type = ETH_RC;
    frame_width = frWidth;
    frame_height = frHeight;
    data_height = dataHeight;
    socket_descriptor = Descriptor;
    pixel_size = 2;
}

RemoteCamera::~RemoteCamera()
{
    running.store(false);
    emit timeout();
    is_connected = false;
    readLoopFuture.waitForFinished();
}

bool RemoteCamera::start()
{
    qDebug() << "Attempting Connection";
    socket = new QTcpSocket();
    socket->setSocketDescriptor(socket_descriptor);
    socket->setReadBufferSize(framesize*2*4);
    qDebug() << socket->readAll(); // I need to do this to make sure I don't miss anything (according to sources :))

    connect(socket, &QTcpSocket::stateChanged, this, &RemoteCamera::SocketStateChanged);
    if (!socket->waitForConnected(1000))
    {
        qDebug() << "Could not complete descriptor pass-off";
        return false;
    }
    is_connected = true;
    data_socket.setDevice(socket); // This is what we are reading from

    is_receiving = false;
    window_initialized = false;

    header.resize(size_t(headsize));
    std::fill(header.begin(), header.end(), 0);

    dummy.resize(size_t(frame_width * data_height));
    std::fill(dummy.begin(), dummy.end(), 0);
    for (int n = 0; n < nFrames; n++) { // Not necessary for non-queued approach
        frame_buf.emplace_back(std::vector<uint16_t>(size_t(frame_width * data_height), 0));
    }

    temp_frame.resize(size_t(frame_width * data_height));
    std::fill(temp_frame.begin(), temp_frame.end(), 0);
    running.store(true);

    return true;
}

void RemoteCamera::SocketRead()
{
    uint32_t byte_pos = 0;
    /*if (!socket->waitForReadyRead(500)) { // If it timed out return existing frame
        if (!(socket->bytesAvailable() >= pixel_size)) {
            qDebug() << "Timed Out" << pixel_pos;
            return;
        }
    }

    while(pixel_pos < framesize) { // Could cause indefinite wait, add timeout
        //qDebug() << "Bytes" << socket->bytesAvailable();
        //if (socket->bytesAvailable() >= pixel_size) {
        uint16_t temp_int;
        data_socket >> temp_int;
        temp_frame[pixel_pos] = (temp_int >> 8) | ( temp_int << 8); // Bits are interpretted as mid-little endian, so we just shift them back
        pixel_pos++;
        //}
    }
    qDebug() << "Bytes" << socket->bytesAvailable() << pixel_pos;*/
    uint32_t frame_byte_size = framesize*2; // Two bytes per pixel
    uint32_t min_read_size = frame_byte_size/32;
//    QByteArray buffer(frame_byte_size, 0);
//    unsigned char odd_byte_left = 0;
//    bool odd_byte = false;
    qint64 bytes_read = 0;
    char *receive = (char *)calloc(frame_byte_size, sizeof (char));
    do {
        if (!socket->waitForReadyRead(500)) { // If it timed out return existing frame
            if (!(socket->bytesAvailable() > 0)) {
                qDebug() << "Timed Out" << byte_pos;
                is_receiving = false;
                break;
            }
        }
//        if (odd_byte) { // Copy the
//            receive[0] = odd_byte_left;
//        }
        //qDebug() << "Bytes: " << byte_pos << socket->bytesAvailable();
        if (socket->bytesAvailable() < 0){
            qDebug() << "NEGATIVE BYTES AVAILABLE *********";
            break;
        } else if (socket->bytesAvailable() <= min_read_size && byte_pos < (frame_byte_size - min_read_size)) {
            continue;
        }
        bytes_read = socket->read(receive, std::min((frame_byte_size - byte_pos), (unsigned int)socket->bytesAvailable()));
        //qDebug() << "Read: " << byte_pos << bytes_read << socket->bytesAvailable();
        std::memcpy((char*)temp_frame.data() + byte_pos, receive, bytes_read);
//        QByteArray read_data(frame_byte_size, 0);

//        std::memcpy(read_data.data(), buffer.data(), buffer.size());
//        size_t dataSize = buffer.size();
//        qDebug() << "Datastream: " << dataSize << pixel_pos*2 << odd_byte;
//        QDataStream dstream(&buffer, QIODevice::ReadOnly); // Pretty sure error is happening here.
//        uint32_t i;
//        for (i = 0; i < (bytes_read >> 1); i++) { // Go through each pixel in the receive buffer
//            uint16_t temp_int;
////            dstream >> temp_int; // Each pixel is 2 bytes
//            temp_int = (receive[i*2] >> 8) | ( receive[i*2 + 1] << 8);
//            temp_frame[pixel_pos + i] = temp_int; // Bits are interpretted as mid-little endian, so we just shift them back
//            qDebug() << "pixel" << temp_int;
//        }
//        if (i*2 < bytes_read) { // Handle the case where we read an odd number of bytes
//            odd_byte_left = (receive[i*2] >> 8) | ( receive[i*2 + 1] << 8);
//            odd_byte = true;
//        } else {
//            odd_byte = false;
//        }

        byte_pos += bytes_read;
        //qDebug() << "Read pixels from socket" << pixel_pos << odd_byte << dataSize;
    } while (byte_pos < frame_byte_size && is_connected); // While we still have more pixels
    free(receive);
    qDebug() << "Finished Receiving Frame: " << byte_pos;
}

uint16_t* RemoteCamera::getFrame()
{
    // Prompt the server to send a frame over
    if(is_connected && window_initialized) {
        qDebug() << "Can Receive" << is_receiving;
        if(socket->isWritable() && !is_receiving) { // Validate that socket is ready
            is_receiving = true; // Forces only one request to go out at a time
            qDebug() << "Getting frame from socket..." << image_no;
            int written = socket->write("Ready");
            if (written == -1) {
                qDebug() << "Failed to write...";
                return temp_frame.data();
            }
            socket->waitForBytesWritten(100);
            socket->flush();
            this->SocketRead();
            is_receiving = false;
        }
        qDebug() << image_no << "- Image Received";
        image_no ++;
        return temp_frame.data();
    } else {
        //qDebug() << "Returning Dummy Data";
        qDebug() << image_no << "Dummy Returned";
        return dummy.data();
    }
}

void RemoteCamera::SocketStateChanged(QTcpSocket::SocketState state)
{
    qDebug() << "Socket has changed state" << state;
    switch (state) {
    case QTcpSocket::ConnectedState:
        // Do nothing, we should be in this state. But investigate we should never be going back here
        break;
    case QTcpSocket::UnconnectedState: // We might not want to quit once we enter this state, because we might recover.
        qDebug() << "Unconnected State";
        is_connected = false;
        emit timeout();
        break;
    default:
        qDebug() << "WEIRD STATE ENCOUNTERED. QUITTING" << state;
        is_connected = false;
        socket->disconnect();
        emit timeout();
        break;
    }
}
