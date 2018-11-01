#ifndef CAMERAVIEWDIALOG_H
#define CAMERAVIEWDIALOG_H

#include <QDialog>
#include <QStringList>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "cameramodel.h"

class CameraViewDialog : public QDialog
{
    Q_OBJECT

public:
    CameraViewDialog(CameraModel *camera)
    {
        this->setWindowTitle("Detected Camera Information");

        QPushButton *okButton = new QPushButton("&Ok", this);
        connect(okButton, &QPushButton::clicked, this, &QDialog::accept);

        QString infoList;
        switch(camera->getSourceType()) {
#if !(__MACH__ || __APPLE__)
        case CAMERA_LINK:
            infoList = QString(camera->getCameraName());
            break;
#endif
        case SSD:
            infoList = "XIO file reader";
            break;
        case DEBUG:
            infoList = "ENVI file reader";
            break;
        default:
            qDebug() << "hello, world";
        }

        QLabel *infoLabel = new QLabel(QString("Camera type: %1").arg(infoList));
        QLabel *geomLabel = new QLabel(
                    QString("Frame Geometry (height x width): %1 x %2").arg(QString::number(camera->getFrameHeight()),
                                                                            QString::number(camera->getFrameWidth())));

        QVBoxLayout *dialogLayout = new QVBoxLayout(this);
        dialogLayout->addWidget(new QLabel("Camera geometry currently in use:"));
        dialogLayout->addWidget(infoLabel);
        dialogLayout->addWidget(geomLabel);
        dialogLayout->addWidget(okButton);
    }

};

#endif // CAMERAVIEWDIALOG_H
