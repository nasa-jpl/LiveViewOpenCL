#ifndef SAVECLIENT_H
#define SAVECLIENT_H

#include <QCoreApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QRunnable>
#include <QtNetwork/QTcpSocket>

#include "image_type.h"

class SaveClient : public QObject, public QRunnable
{
    Q_OBJECT

public:
    explicit SaveClient(qintptr socket_desc, QObject *parent = nullptr);

signals:
    void saveFrames(save_req_t);

protected:
    void run();

private:
    qintptr socketDescriptor;
    bool connected;
};

#endif // SAVECLIENT_H
