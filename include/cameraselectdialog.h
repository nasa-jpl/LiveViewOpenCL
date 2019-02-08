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
#include <QLineEdit>

class CameraSelectDialog : public QDialog
{
    Q_OBJECT

public:
    CameraSelectDialog(QSettings *set) : s(set)
    {
        this->setWindowTitle("Select Camera Model");
        this->cameraList = (QStringList()
#if !(__MACH__ || __APPLE__)
                            << QString("CL")
#endif
                            << QString("SSD (ENVI)"))
                            << QString("SSD (XIO)");

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

        dim_dialog = new QDialog;
        dim_dialog->setWindowTitle("Select Input Dimensions");

        horizontal = new QLineEdit;
        horizontal->setText(s->value(QString("ssd_x"), "").toString());

        vertical = new QLineEdit;
        vertical->setText(s->value(QString("ssd_y"), "").toString());

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

        QHBoxLayout *xLayout = new QHBoxLayout;
        xLayout->addWidget(new QLabel("X:"));
        xLayout->addWidget(horizontal);

        QHBoxLayout *yLayout = new QHBoxLayout;
        yLayout->addWidget(new QLabel("Y:"));
        yLayout->addWidget(vertical);

        QVBoxLayout *dimDialogLayout = new QVBoxLayout(dim_dialog);
        dimDialogLayout->addLayout(xLayout);
        dimDialogLayout->addLayout(yLayout);
        dimDialogLayout->addLayout(dimButtonLayout);

    }

private slots:
    void okButtonPressed()
    {
        s->setValue(QString("cam_model"),
                    source_t_name[cameraListModel->data(
                        cameraListView->selectionModel()->selectedIndexes()[0],
                        Qt::DisplayRole).toString().toStdString()]);
        s->setValue(QString("show_cam_dialog"), doNotShowBox->checkState() == 0);
        if (s->value(QString("cam_model"), "SSD").toInt() == 0) {
            dim_dialog->exec();
        } else {
            this->accept();
        }
    }

    void dim_accept()
    {
        s->setValue(QString("ssd_x"), horizontal->text());
        s->setValue(QString("ssd_y"), vertical->text());
        this->accept();
    }

private:
    std::unordered_map<std::string, source_t> source_t_name{{"SSD (ENVI)", SSD}, {"SSD (XIO)", SSD}, {"CL", CAMERA_LINK}, {"CAMERA_LINK", CAMERA_LINK}, {"Debug", DEBUG}};
    LVMainWindow *w;
    QSettings *s;
    QStringList cameraList;
    QStringList formatList;
    QListView *cameraListView;
    QListView *formatListView;
    QStringListModel *cameraListModel;
    QStringListModel *formatListModel;
    QCheckBox *doNotShowBox;
    QDialog *dim_dialog;
    QLineEdit *horizontal;
    QLineEdit *vertical;
};

#endif // CAMERASELECTDIALOG_H
