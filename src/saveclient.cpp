#include "saveclient.h"

SaveClient::SaveClient(qintptr socket_desc, QObject *parent) :
    QObject(parent), socketDescriptor(socket_desc), connected(true)
{
}

void SaveClient::run()
{
    QTcpSocket *clientConnection = new QTcpSocket();
    clientConnection->setSocketDescriptor(socketDescriptor);
    clientConnection->waitForConnected();
    connect(clientConnection, &QTcpSocket::disconnected, this, [=](){
        connected = false;
    });

    while (connected) {
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
                    clientConnection->waitForBytesWritten();
                } // else if other request types...
             } else {
                // If there is no requestType, return an error to the client.
                responseObj["status"] = 502;
                responseObj["message"] = "Invalid client request, no requestType found.";
                qWarning("Invalid client request, no requestType found.");
                responseDoc.setObject(responseObj);
                clientConnection->write(qCompress(responseDoc.toJson()));
            }
        } else {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
    }
    clientConnection->disconnect();
    clientConnection->deleteLater();
}
