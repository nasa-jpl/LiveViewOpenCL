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

void RemoteCamera::SocketRead()
{
    uint32_t byte_pos = 0; // Two bytes per pixel
    uint32_t frame_byte_size = framesize*2;
    do {
        if (!socket->waitForReadyRead(2000)) // If it timed out
        {
            qDebug() << "Timed out" << byte_pos;
            is_receiving = false;
            break; // Return existing frame if we wait too long
        }
        // Convert the data
        QByteArray buffer = socket->read(frame_byte_size - byte_pos);
        size_t dataSize = buffer.size();
        QDataStream dstream(buffer);
        for (uint32_t i = byte_pos; i < dataSize + byte_pos; i++) // Go through each byte in the message
        {
            uint16_t temp_int;
            dstream >> temp_int;
            temp_frame[i] = (temp_int >> 8) | ( temp_int << 8); // Bits are interpretted as mid-little endian, so we just shift them back
        }
        byte_pos += dataSize;
    } while (byte_pos < frame_byte_size);
}

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
            this->SocketRead();

            for (size_t i = 0; i < dataSize; i++) { dstream >> temp_frame[i]; } // Do I need a for loop?
            //qDebug() << "Returning Data";
            image_no ++;
            is_receiving = false;
        }
        qDebug() << image_no << "- Image Received";
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
