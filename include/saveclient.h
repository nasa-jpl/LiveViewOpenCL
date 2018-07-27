#ifndef SAVECLIENT_H
#define SAVECLIENT_H

#include <QObject>
#include <QtNetwork/QTcpSocket>
#include <QDebug>

class SaveClient : public QObject
{
    Q_OBJECT

public:
    explicit SaveClient(QTcpSocket *socket, QObject *parent = 0) :
        QObject(parent), clientConnection(socket)
    {
        connect(clientConnection, &QAbstractSocket::connected,
                this, &SaveClient::connectClient);
        connect(clientConnection, &QAbstractSocket::disconnected,
                clientConnection, &QObject::deleteLater);
        connect(clientConnection, &QAbstractSocket::readyRead,
                this, &SaveClient::receiveAction);
    }

public slots:
    void connectClient()
    {
        qDebug() << "Starting to read data";
    }

    void receiveAction()
    {
        if (clientConnection->bytesAvailable() > (qint64)sizeof(quint16)) {
            QByteArray clientRequest = clientConnection->readAll();
            qDebug() << "Got:" << clientRequest;
        }
    }

private:
    QTcpSocket *clientConnection;
};

#endif // SAVECLIENT_H
