#ifndef SAVECLIENT_H
#define SAVECLIENT_H

#include <QObject>
#include <QtNetwork/QTcpSocket>
#include <QDebug>

#include <QJsonDocument>
#include <QJsonObject>

#include "image_type.h"

class SaveClient : public QObject
{
    Q_OBJECT

public:
    explicit SaveClient(QTcpSocket *socket, QObject *parent = nullptr) :
        QObject(parent), clientConnection(socket)
    {
        connect(clientConnection, &QAbstractSocket::disconnected,
                clientConnection, &QObject::deleteLater);
        connect(clientConnection, &QAbstractSocket::readyRead,
                this, &SaveClient::receiveAction);
    }

signals:
    void saveFrames(save_req_t);

public slots:
    void receiveAction()
    {
        if (clientConnection->bytesAvailable() > static_cast<qint64>(sizeof(quint16))) {
            const QByteArray clientRequest = qUncompress(clientConnection->readAll());
            const QJsonObject rootObj = QJsonDocument::fromJson(clientRequest).object();
            QJsonObject responseObj;
            QJsonDocument responseDoc;
            if (rootObj.contains("requestType") && rootObj["requestType"].isString()) {
                const QString &requestType = rootObj["requestType"].toString();
                if (QString::compare(requestType, QString("\"Save\""), Qt::CaseInsensitive)) {
                    const std::string &fname = rootObj["fileName"].toString().toStdString();
                    const int64_t &nFrames = rootObj["numFrames"].toInt();
                    const int64_t &nAvgs = rootObj.contains("numAvgs") ? rootObj["numAvgs"].toInt() : 1;
                    save_req_t new_req = {fwBIL, fname, nFrames, nAvgs};
                    emit saveFrames(new_req);
                    responseObj["status"] = 200;
                    responseObj["message"] = "OK";
                    responseDoc.setObject(responseObj);
                    clientConnection->write(qCompress(responseDoc.toJson()));
                    return;
                } // else if other request types...
            } else {
                // If there is no requestType, return an error to the client.
                responseObj["status"] = 502;
                responseObj["message"] = "Invalid client request, no requestType found.";
                qWarning("Invalid client request, no requestType found.");
                responseDoc.setObject(responseObj);
                clientConnection->write(qCompress(responseDoc.toJson()));
            }
        }
        // else, an empty request was received. No need to do anything.
    }

private:
    QTcpSocket *clientConnection;
};

#endif // SAVECLIENT_H
