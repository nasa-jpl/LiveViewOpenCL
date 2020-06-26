#ifndef CAMERASELECTDIALOG_H
#define CAMERASELECTDIALOG_H

#include "lvmainwindow.h"
#include "string.h"

#include <QDebug>
#include <QDialog>
#include <QStringList>
#include <QLabel>
#include <QListView>
#include <QStringListModel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSettings>
#include <QCheckBox>
#include <QLineEdit>
#include <QTcpSocket>
#include <QHostAddress>
#include <QByteArray>

class CameraSelectDialog : public QDialog
{
    Q_OBJECT

public:
    CameraSelectDialog(QSettings *set) : s(set)
    {
        this->setWindowTitle("Select Camera Model");
        this->cameraList = ((QStringList()
#ifdef USE_EDT
                            << QString("CL")
#endif
                            << QString("SSD (ENVI)"))
                            << QString("SSD (XIO)"))
                            << QString("RC (Remote Camera)"); // Remote Camera

        cameraListModel = new QStringListModel(this);
        cameraListModel->setStringList(cameraList);

        cameraListView = new QListView(this);
        cameraListView->setModel(cameraListModel);
        cameraListView->setSelectionMode(QAbstractItemView::SingleSelection);
        cameraListView->setCurrentIndex(cameraListModel->index(0));

        QPushButton *okButton = new QPushButton("&Ok", this);
        connect(okButton, &QPushButton::clicked, this,
                &CameraSelectDialog::okButtonPressed);

        QPushButton *cancelButton = new QPushButton("&Cancel", this);
        connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

        doNotShowBox = new QCheckBox("Do Not Show Again", this);
        doNotShowBox->setChecked(!s->value(QString("show_cam_dialog"), false).toBool());

        QHBoxLayout *buttonLayout = new QHBoxLayout;
        buttonLayout->addWidget(okButton);
        buttonLayout->addWidget(cancelButton);

        QVBoxLayout *dialogLayout = new QVBoxLayout(this);
        dialogLayout->addWidget(new QLabel("Select a Camera Model"));
        dialogLayout->addWidget(cameraListView);
        dialogLayout->addWidget(doNotShowBox);
        dialogLayout->addLayout(buttonLayout);

        // Dialog for getting image dimensions
        dim_dialog = new QDialog;
        dim_dialog->setWindowTitle("Set Input Dimensions");

        horizontal = new QLineEdit;
        horizontal->setText(s->value(QString("ssd_width"), "").toString());

        vertical = new QLineEdit;
        vertical->setText(s->value(QString("ssd_height"), "").toString());

        QPushButton *okDimButton = new QPushButton("&Ok", dim_dialog);
        connect(okDimButton, &QPushButton::clicked,
                dim_dialog, &QDialog::accept);
        connect(okDimButton, &QPushButton::clicked,
                this, &CameraSelectDialog::dim_accept);

        QPushButton *cancelDimButton = new QPushButton("&Cancel", dim_dialog);
        connect(cancelDimButton, &QPushButton::clicked, dim_dialog, &QDialog::reject);

        QHBoxLayout *dimButtonLayout = new QHBoxLayout;
        dimButtonLayout->addWidget(okDimButton);
        dimButtonLayout->addWidget(cancelDimButton);

        QGridLayout *dimmaDialogLayout = new QGridLayout;
        dimmaDialogLayout->addWidget(new QLabel("Height (px):"), 1, 1, 1, 1);
        dimmaDialogLayout->addWidget(vertical, 1, 2, 1, 1);
        dimmaDialogLayout->addWidget(new QLabel("Width (px):"), 2, 1, 1, 1);
        dimmaDialogLayout->addWidget(horizontal, 2, 2, 1, 1);

        // you have to say the variable names out loud (^_^)
        QVBoxLayout *dimDamDimmaDialogLayout = new QVBoxLayout(dim_dialog);
        dimDamDimmaDialogLayout->addLayout(dimmaDialogLayout);
        dimDamDimmaDialogLayout->addLayout(dimButtonLayout);


        // Dialog for getting desired server IP
        ip_dialog = new QDialog;
        ip_dialog->setWindowTitle("Configure IP Connection");

        QString IpRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
        QRegularExpression IpRegex ("^" + IpRange
                                    + "(\\." + IpRange + ")"
                                    + "(\\." + IpRange + ")"
                                    + "(\\." + IpRange + ")$");
        QRegularExpressionValidator *ipValidator = new QRegularExpressionValidator(IpRegex, this);

        ip_addr = new QLineEdit;
        ip_addr->setPlaceholderText("127.0.0.1");
        ip_addr->setValidator(ipValidator);

        QIntValidator *portValidator = new QIntValidator(0, 99999, this);
        ip_port = new QLineEdit;
        ip_port->setPlaceholderText("69696");
        ip_port->setValidator(portValidator);

        okIpButton = new QPushButton("&Done", ip_dialog);
        okIpButton->setEnabled(false); // Default state should be disabled
        connect(okIpButton, &QPushButton::clicked,
                ip_dialog, &QDialog::accept);
        connect(okIpButton, &QPushButton::clicked,
                this, &CameraSelectDialog::connection_accept);

        QPushButton *connectIpButton = new QPushButton("&Connect", ip_dialog);
        connect(connectIpButton, &QPushButton::clicked,
                this, &CameraSelectDialog::ip_connect);

        QPushButton *cancelIpButton = new QPushButton("&Cancel", ip_dialog);
        connect(cancelIpButton, &QPushButton::clicked, ip_dialog, &QDialog::reject);

        QHBoxLayout *ipButtonLayout = new QHBoxLayout;
        ipButtonLayout->addWidget(okIpButton);
        ipButtonLayout->addWidget(cancelIpButton);

        QGridLayout *ipmaDialogLayout = new QGridLayout;
        ipmaDialogLayout->addWidget(new QLabel("Ip Address:"), 1, 1, 1, 1);
        ipmaDialogLayout->addWidget(new QLabel("Port:"), 1, 2, 1, 1);
        ipmaDialogLayout->addWidget(ip_addr, 2, 1, 1, 1);
        ipmaDialogLayout->addWidget(ip_port, 2, 2, 1, 1);
        ipmaDialogLayout->addWidget(connectIpButton, 3, 1, 1, 1);

        statusLabel = new QLabel(s->value(QString("connection_status"), "").toString());
        heightLabel = new QLabel(s->value(QString("received_height"), "").toString());
        widthLabel = new QLabel(s->value(QString("received_width"), "").toString());
        ipmaDialogLayout->addWidget(new QLabel("Status:"), 4, 1, 1, 1);
        ipmaDialogLayout->addWidget(statusLabel, 4, 2, 1, 1);
        ipmaDialogLayout->addWidget(new QLabel("Height (px):"), 5, 1, 1, 1);
        ipmaDialogLayout->addWidget(heightLabel, 5, 2, 1, 1);
        ipmaDialogLayout->addWidget(new QLabel("Width (px):"), 6, 1, 1, 1);
        ipmaDialogLayout->addWidget(widthLabel, 6, 2, 1, 1);

        QVBoxLayout *ipDamDimmaDialogLayout = new QVBoxLayout(ip_dialog);
        ipDamDimmaDialogLayout->addLayout(ipmaDialogLayout);
        ipDamDimmaDialogLayout->addLayout(ipButtonLayout);

    }

private slots:
    void okButtonPressed()
    {
        s->setValue(QString("cam_model"),
                    source_t_name[cameraListModel->data(
                        cameraListView->selectionModel()->selectedIndexes()[0],
                        Qt::DisplayRole).toString().toStdString()]);
        s->setValue(QString("show_cam_dialog"), doNotShowBox->checkState() == 0);
        if (s->value(QString("cam_model"), "XIO").toInt() == 0 ||
            s->value(QString("cam_model"), "ENVI").toInt() == 1) {
            dim_dialog->exec();
        } else if (s->value(QString("cam_model"), "RC").toInt() == 3) {
            s->setValue(QString("connection_status"), "Not Connected");
            s->setValue(QString("received_height"), "");
            s->setValue(QString("received_width"), "");
            ip_dialog->exec();
        } else {
            this->accept();
        }
    }

    void dim_accept()
    {
        s->setValue(QString("ssd_width"), horizontal->text());
        s->setValue(QString("ssd_height"), vertical->text());
        this->accept();
    }

    void ip_connect() // This establishes a connection to the server and gets the information like size as the handshake.
    {
        // All of this will be turned into an object
        s->setValue(QString("ip_address"), ip_addr->text() + ip_port->text());
        qDebug() << "Connecting to " << ip_addr->text() << ":" << ip_port->text();

        QTcpSocket *connection = new QTcpSocket();
        connection->connectToHost(QHostAddress(ip_addr->text()),ip_port->text().toUShort());

        if(connection->waitForConnected(1000)) { // Wait for connection
            qDebug() << "Connected!";
            connection->write("Hey ;)");
            connection->waitForBytesWritten();
        } else {
            connection->close();
            statusLabel->setText("Connection Failed");
            return;
        }
        qDebug() << "After Connected";

        if(connection->waitForReadyRead(10000)) { // Wait for reading to be ready
            //qDebug() << connection->readAll();
            const QByteArray connectionMessage = qUncompress(connection->readAll());
            const QJsonObject messageObj = QJsonDocument::fromJson(connectionMessage).object();
            if (messageObj.contains("requestType") && messageObj["requestType"].isString()) {
                const QString &requestType = messageObj["requestType"].toString();
                const double &requestHeight = messageObj["height"].toDouble();
                const double &requestWidth = messageObj["width"].toDouble();
                if (QString::compare(requestType, QString("\"Handshake\""), Qt::CaseInsensitive)) {
                    // Connection confirmed, set handshake to true
                    statusLabel->setText("Connected");
                    heightLabel->setText(QString::number(requestHeight));
                    widthLabel->setText(QString::number(requestWidth));
                    s->setValue(QString("socket_descriptor"), connection->socketDescriptor()); // Should this be done during connection_accept()
                    okIpButton->setEnabled(true);
                    return;
                }
            } else {
                // If there is no requestType, return an error to the client.
                connection->close();
                statusLabel->setText("Handshake Failed");
                qDebug() << "Got invalid handshake.";
                return;
            }
        } else {
            connection->close();
            statusLabel->setText("Handshake Failed");
            qDebug() << "Did not get handshake response from server.";
            return;
        }
    }

    void connection_accept()
    {
        s->setValue(QString("ssd_width"), widthLabel->text());
        s->setValue(QString("ssd_height"), heightLabel->text());
        this->accept();
    }

private:
    std::unordered_map<std::string, source_t> source_t_name{{"SSD (ENVI)", ENVI}, {"SSD (XIO)",XIO}, {"CL", CAMERA_LINK}, {"CAMERA_LINK", CAMERA_LINK}, {"RC (Remote Camera)", RC}};
    QSettings *s;
    QStringList cameraList;
    QStringList formatList;
    QListView *cameraListView;
    QStringListModel *cameraListModel;
    QCheckBox *doNotShowBox;
    QDialog *dim_dialog;
    QDialog *ip_dialog;
    QLineEdit *horizontal;
    QLineEdit *vertical;
    QLineEdit *ip_addr;
    QLineEdit *ip_port;
    QLabel *statusLabel;
    QLabel *heightLabel;
    QLabel *widthLabel;
    QPushButton *okIpButton;
};

#endif // CAMERASELECTDIALOG_H
