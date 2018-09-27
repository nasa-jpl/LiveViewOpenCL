#include "saveserver.h"
#include "saveclient.h"

SaveServer::SaveServer(QObject *parent)
    : QObject(parent),  port(50000),
      tcpServer(nullptr), networkSession(nullptr)
{
    QThreadPool::globalInstance()->setMaxThreadCount(5);

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("LiveView"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
                QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        networkSession = new QNetworkSession(config, this);
        connect(networkSession, &QNetworkSession::opened, this, &SaveServer::sessionOpened);

        networkSession->open();
    } else {
        sessionOpened();
    }

    connect(tcpServer, &QTcpServer::newConnection, this, &SaveServer::startClient);
}

void SaveServer::sessionOpened()
{
    // Save the used configuration
    if (networkSession) {
        QNetworkConfiguration config = networkSession->configuration();
        QString id;
        if (config.type() == QNetworkConfiguration::UserChoice) {
            id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
        } else {
            id = config.identifier();
        }

        QSettings settings(QSettings::UserScope, QLatin1String("LiveView"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
        settings.endGroup();
    }

    tcpServer = new QTcpServer(this);

    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (QHostAddress &address: ipAddressesList) {
        if (address != QHostAddress::LocalHost && address.toIPv4Address()) {
            ipAdress = address;
            break;
        }
    }

    // if we did not find one, use IPv4 localhost
    if (ipAdress.isNull()) {
        ipAdress = QHostAddress(QHostAddress::LocalHost);
    }

    if (!tcpServer->listen(ipAdress, port)) {
        qDebug() << "Unable to start server:" << tcpServer->errorString();
        return;
    }

    qDebug() << "Host address is" << ipAdress.toString();
    qDebug() << "Port is" << tcpServer->serverPort();
}

void SaveServer::startClient()
{
    SaveClient *client = new SaveClient(tcpServer->nextPendingConnection());
    connect(client, &SaveClient::saveFrames, this, [&](save_req_t req)
    {
        emit startSavingRemote(req);
    });
}
