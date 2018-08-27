#ifndef CAMERASELECTDIALOG_H
#define CAMERASELECTDIALOG_H

#include "lvmainwindow.h"

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

class CameraSelectDialog : public QDialog
{
    Q_OBJECT

public:
    CameraSelectDialog(QSettings *set) : s(set)
    {
        this->cameraList = (QStringList() << QString("CL") << QString("SSD"));
        this->setWindowTitle("Select Camera Model");

        cameraListView = new QListView(this);
        cameraListModel = new QStringListModel(this);
        cameraListModel->setStringList(cameraList);
        cameraListView->setModel(cameraListModel);
        cameraListView->setSelectionMode(QAbstractItemView::SingleSelection);
        cameraListView->setCurrentIndex(cameraListModel->index(0));

        QPushButton *okButton = new QPushButton("&Ok", this);
        connect(okButton, &QPushButton::clicked, this,
                &CameraSelectDialog::okButtonPressed);

        QPushButton *cancelButton = new QPushButton("&Cancel", this);
        connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

        doNotShowBox = new QCheckBox("Do Not Show Again", this);
        if (s->value(QString("show_cam_dialog"), true).toBool()) {
            doNotShowBox->setChecked(false);
        } else {
            doNotShowBox->setChecked(true);
        }

        QHBoxLayout *buttonLayout = new QHBoxLayout;
        buttonLayout->addWidget(okButton);
        buttonLayout->addWidget(cancelButton);

        QVBoxLayout *dialogLayout = new QVBoxLayout(this);
        dialogLayout->addWidget(new QLabel("Select a Camera Model"));
        dialogLayout->addWidget(cameraListView);
        dialogLayout->addWidget(doNotShowBox);
        dialogLayout->addLayout(buttonLayout);
    }

    LVMainWindow *w;
    QSettings *s;
    QStringList cameraList;
    QListView *cameraListView;
    QStringListModel *cameraListModel;
    QCheckBox *doNotShowBox;

private slots:
    void okButtonPressed()
    {
        s->setValue(QString("cam_model"),
                    cameraListModel->data(
                        cameraListView->selectionModel()->selectedIndexes()[0],
                        Qt::DisplayRole));
        s->setValue(QString("show_cam_dialog"), doNotShowBox->checkState() == 0);
        this->accept();
    }
};

#endif // CAMERASELECTDIALOG_H
