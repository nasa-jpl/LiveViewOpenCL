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
    frame_byte_size = framesize*pixel_size;
}

RemoteCamera::~RemoteCamera()
{
    running.store(false);
    emit timeout();
    is_connected = false;
    free(temp_frame_array);
    readLoopFuture.waitForFinished();
}

bool RemoteCamera::start()
{
    qDebug() << "Attempting Connection";
    qRegisterMetaType<QAbstractSocket::SocketState>();
    socket = new QTcpSocket();
    socket->setSocketDescriptor(socket_descriptor);
    socket->setReadBufferSize(framesize*2*4);
    //qDebug() << socket->readAll(); // I need to do this to make sure I don't miss anything (according to sources :))

    connect(socket, &QTcpSocket::stateChanged, this, &RemoteCamera::SocketStateChanged);
    //connect(socket, &QTcpSocket::readyRead, this, &RemoteCamera::SocketRead);
    if (!socket->waitForConnected(1000))
    {
        qDebug() << "Could not complete descriptor pass-off";
        return false;
    }
    is_connected = true;
    //data_socket.setDevice(socket); // This is what we are reading from

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

qint64 RemoteCamera::SafeRead(char *read_buffer, int max_bytes) {
    return socket->read(read_buffer, max_bytes);
}

qint64 RemoteCamera::SafeWrite(char *write_buffer) {
    quint64 status = socket->write(write_buffer);
    socket->waitForBytesWritten(100);
    socket->flush();
    return status;
}

void RemoteCamera::SocketRead()
{
    size_t byte_pos = 0;
    uint32_t min_read_size = 1448; // Packet size
    qint64 bytes_read = 0;
    do {
        if (!socket->waitForReadyRead(500)) { // If it timed out return existing frame
            if (!(socket->bytesAvailable() > 0)) {
                qDebug() << "Timed Out" << byte_pos;
                break;
            }
        }

        socket->startTransaction(); // Transaactions aren't confirmed to work.
        bytes_read = socket->read((char*)temp_frame.data() + byte_pos, frame_byte_size - byte_pos);
        //qDebug() << "Read" << (bytes_read >> 1) << "pixels into pos" << (byte_pos >> 1) << "to" << ((byte_pos + bytes_read)>>1);
        if (bytes_read < 0) {
            qDebug() << "Reading Failed" << bytes_read << errno;
            break;
        }
        socket->commitTransaction();
        byte_pos += bytes_read;

    } while (byte_pos < frame_byte_size && is_connected); // While we still have more pixels
    qDebug() << "Finished Receiving Frame: " << byte_pos;

    if (socket->bytesAvailable() > 0) { // Just output if all the bytes weren't used.
        qDebug() << "Bytes left over:" << socket->bytesAvailable();
    }
}

uint16_t* RemoteCamera::getFrame()
{
    // Prompt the server to send a frame over
    if(is_connected && window_initialized) {
        qDebug() << "Can Receive" << is_receiving;
        if(socket->isWritable() && !is_receiving) { // Validate that socket is ready
            is_receiving = true; // Forces only one request to go out at a time
            qDebug() << "Getting frame from socket..." << image_no;
            int written = SafeWrite((char*)"ReadyFrame");
            if (written == -1) {
                qDebug() << "Failed to write...";
                return temp_frame.data();
            }

            this->SocketRead();
            is_receiving = false;
        }
        qDebug() << image_no << "- Image Received";
        image_no ++;
        return temp_frame.data();
//        return temp_frame_array;
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
    case QTcpSocket::ClosingState:
    case QTcpSocket::UnconnectedState: // We might not want to quit once we enter this state, because we might recover.
        qDebug() << "Unconnected State";
        is_connected = false;
        socket->disconnect();
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
