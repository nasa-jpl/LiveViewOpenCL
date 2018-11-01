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

        QLabel *infoLabel = new QLabel(QString("Camera type: %1").arg(camera->getCameraType()));
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
