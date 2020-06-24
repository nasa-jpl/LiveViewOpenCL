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

    socket = new QTcpSocket();
    socket->setSocketDescriptor(Descriptor);
    qDebug() << socket->readAll(); // I need to do this to make sure I don't miss anything (according to sources :))

    connect(socket, &QTcpSocket::stateChanged, this, &RemoteCamera::SocketStateChanged);
    socket->waitForConnected();
    is_connected = true;
    qDebug() << "Waiting for connected" << socket->state(); // This line is required to check that we are still connected.

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
}

RemoteCamera::~RemoteCamera()
{
    running.store(false);
    emit timeout();
    is_connected = false;
    readLoopFuture.waitForFinished();
}

uint16_t* RemoteCamera::getFrame()
{
    // Prompt the server to send a frame over
    if(is_connected && window_initialized)
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
                is_receiving = false;
                return temp_frame.data(); // Return existing frame
            }
            //qDebug() << "Waited for read";

            // Convert the data
            QByteArray buffer = socket->readAll();
            size_t dataSize = buffer.size();
            QDataStream dstream(buffer);
            for (size_t i = 0; i < dataSize; i++)
            {
                uint16_t temp_int;
                dstream >> temp_int;
                temp_frame[i] = (temp_int >> 8) | ( temp_int << 8); // Bits are interpretted as mid-little endian, so we just shift them back
            }

            //qDebug() << "Returning Data";
            is_receiving = false;
            return temp_frame.data();
        } else {
            //qDebug() << "Dummy data";
            //qDebug() << "---";
            return temp_frame.data();
        }
    } else {
        //qDebug() << "Returning Dummy Data";
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
