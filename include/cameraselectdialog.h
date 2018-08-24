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

        QPushButton *okButton = new QPushButton("&Ok", this);
        connect(okButton, &QPushButton::clicked, this,
                &CameraSelectDialog::okButtonPressed);

        QPushButton *cancelButton = new QPushButton("&Cancel", this);
        connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

        QHBoxLayout *buttonLayout = new QHBoxLayout;
        buttonLayout->addWidget(okButton);
        buttonLayout->addWidget(cancelButton);

        QVBoxLayout *dialogLayout = new QVBoxLayout(this);
        dialogLayout->addWidget(new QLabel("Select a Camera Model"));
        dialogLayout->addWidget(cameraListView);
        dialogLayout->addLayout(buttonLayout);
    }

    LVMainWindow *w;
    QSettings *s;
    QStringList cameraList;
    QListView *cameraListView;
    QStringListModel *cameraListModel;

private slots:
    void okButtonPressed()
    {
        s->setValue(QString("cam_model"),
                    cameraListModel->data(
                        cameraListView->selectionModel()->selectedIndexes()[0],
                        Qt::DisplayRole));
        this->accept();
    }
};

#endif // CAMERASELECTDIALOG_H
