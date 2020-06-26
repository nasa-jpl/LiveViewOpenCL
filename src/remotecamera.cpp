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
    connect(socket, &QTcpSocket::readyRead, this, &RemoteCamera::SocketReady);
    socket->waitForConnected();
    is_connected = true;
    qDebug() << "Waiting for connected" << socket->state(); // This line is required to check that we are still connected.

    is_receiving = false;
    window_initialized = false;


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
    is_connected = false;
    readLoopFuture.waitForFinished();
}

void RemoteCamera::SocketReady() // Receives data and puts it on the stack
{
    if(is_connected && socket->isReadable()) { // Not sure if we need this check
        // Add the whole buffer to the queue
        QByteArray buffer = socket->readAll();
        size_t bufferSize = buffer.size();
        qDebug() << "Received Something" << receive_q.size() << bufferSize;
        QDataStream dstream(buffer);
        for (size_t i = 0; i < bufferSize; i++)
        {
            uint16_t temp_int;
            dstream >> temp_int;
            receive_q.push((temp_int >> 8) | ( temp_int << 8)); // Bits are interpretted as mid-little endian, so we just shift them back
        }
    }
}

void RemoteCamera::PopFrame()
{
    if (receive_q.size() >= framesize) // Only if we have enough elements in the queue
    {
        for (size_t i = 0; i < framesize; i++)
        {
            if (receive_q.empty()) {
                qDebug() << "ERROR EMPTY QUEUE";
                break;
            }
            //qDebug() << "pop" << receive_q.size();
            temp_frame[i].store(receive_q.front());
            receive_q.pop();
            //qDebug() << "pop" << receive_q.size();
        }
        image_no++;
        qDebug() << "Image Number:" << image_no << receive_q.size();
    }
}

bool RemoteCamera::RequestFrame() // Prompt the server to send a frame over
{
    if(socket->isWritable()) // Validate that socket is ready
    {
        socket->write("Ready");
        return socket->waitForBytesWritten(100);
    } else {
        return false;
    }
}

uint16_t* RemoteCamera::getFrame() // Returns the most recent frame
{
    if(is_connected && window_initialized)
    {
        qDebug() << "Getting frame" << receive_q.size();
        this->RequestFrame();
        this->PopFrame();
        return temp_frame[i].store.data();

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
