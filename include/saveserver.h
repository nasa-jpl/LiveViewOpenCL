#ifndef SAVESERVER_H
#define SAVESERVER_H

#include <QDebug>
#include <QtNetwork>
#include <QThreadPool>

#include "image_type.h"

class SaveServer : public QObject
{
    Q_OBJECT

public:
    explicit SaveServer(QObject *parent = nullptr);

    QHostAddress ipAdress;
    quint16 port;           // May have a value 0 to 65535

signals:
    void startSavingRemote(save_req_t req);

private slots:
    void sessionOpened();
    void startClient();

private:
    QTcpServer *tcpServer;
    QNetworkSession *networkSession;
};

#endif // SAVESERVER_H
