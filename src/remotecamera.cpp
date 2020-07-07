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
    uint32_t frame_byte_size = framesize*2; // Two bytes per pixel
    uint32_t pixel_pos = 0;
    unsigned char odd_byte_left = 0;
    bool odd_byte = false;
    QByteArray buffer(frame_byte_size, 0);
    do {
        if (!socket->waitForReadyRead(500)) { // If it timed out return existing frame
            if (!(socket->bytesAvailable() > 0)) {
                qDebug() << "Timed Out" << pixel_pos;
                is_receiving = false;
                break;
            }
        }
        //qDebug() << "Bytes: " << buffer.size() << frame_byte_size << pixel_pos*2 << socket->bytesAvailable() << socket->isValid() << socket->readBufferSize();
        buffer = socket->read(std::min((frame_byte_size - pixel_pos*2), (unsigned int)socket->bytesAvailable()));
        if (odd_byte) {
            buffer.prepend(odd_byte_left);
        }
        size_t dataSize = buffer.size();
        QDataStream dstream(buffer);
        for (uint32_t i = pixel_pos; i < (dataSize >> 1) + pixel_pos; i++) { // Go through each pixel in the message
            uint16_t temp_int;
            dstream >> temp_int; // Each pixel is 2 bytes
            temp_frame[i] = (temp_int >> 8) | ( temp_int << 8); // Bits are interpretted as mid-little endian, so we just shift them back
        }
        if (!dstream.atEnd()) { // Handle the case where we read an odd number of bytes
            dstream >> odd_byte_left;
            odd_byte = true;
        } else {
            odd_byte = false;
        }

        pixel_pos += (dataSize >> 1);
        //qDebug() << "Read pixels from socket" << pixel_pos << odd_byte << dataSize;
    } while (pixel_pos < framesize && is_connected); // While we still have more pixels
}

uint16_t* RemoteCamera::getFrame()
{
    // Prompt the server to send a frame over
    if(is_connected && window_initialized)
    {
        if(socket->isWritable() && !is_receiving) // Validate that socket is ready
        {
            is_receiving = true; // Forces only one request to go out at a time
            qDebug() << "Getting frame from socket..." << image_no;
            socket->write("Ready");
            //qDebug() << "Wrote";
            socket->waitForBytesWritten(100);
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
        // Do nothing, we should be in this state no but investigate becuase we would need to be somewhere else
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
