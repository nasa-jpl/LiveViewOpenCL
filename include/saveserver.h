#ifndef SAVESERVER_H
#define SAVESERVER_H

#include <QDebug>
#include <QtNetwork>
#include <QThreadPool>

class SaveServer : public QObject
{
    Q_OBJECT

public:
    explicit SaveServer(QObject *parent = nullptr);

    QHostAddress ipAdress;
    quint16 port;           // May have a value 0 to 65535

signals:
    void startSavingRemote(const QString &unverifiedName,
                           const quint64 &nFrames,
                           const quint64 &numAvgs);

private slots:
    void sessionOpened();
    void startClient();

private:
    QTcpServer *tcpServer;
    QNetworkSession *networkSession;
};

#endif // SAVESERVER_H
