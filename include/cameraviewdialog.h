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
#ifdef USE_EDT
        case CAMERA_LINK:
            infoList = QString(camera->getCameraName());
            break;
#endif
        case XIO:
            infoList = "XIO file reader";
            break;
        case ENVI:
            infoList = "ENVI file reader";
            break;
        default:
            qDebug("警報：無法辨認照相機型號，請選擇別的照相機型號。");
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
