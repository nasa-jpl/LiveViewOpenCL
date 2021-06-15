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
#ifdef USE_EDT
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
        dim_dialog->setWindowTitle("Set Input Dimensions");

        horizontal = new QLineEdit;
        horizontal->setText(s->value(QString("ssd_width"), "").toString());

        vertical = new QLineEdit;
        vertical->setText(s->value(QString("ssd_height"), "").toString());

        //
        // PK 6-9-21 added 'lines per frame' input box widget.
        linesPerFrame = new QLineEdit;
        linesPerFrame->setText(s->value(QString("ssd_lines_per_frame"), "").toString());

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
        dimmaDialogLayout->addWidget(new QLabel("Line Height (px):"), 1, 1, 1, 1);
        dimmaDialogLayout->addWidget(vertical, 1, 2, 1, 1);
        dimmaDialogLayout->addWidget(new QLabel("Line Width (px):"), 2, 1, 1, 1);
        dimmaDialogLayout->addWidget(horizontal, 2, 2, 1, 1);

        //
        // PK 6-9-21 added 'lines per frame' input box widget.
        dimmaDialogLayout->addWidget(new QLabel("Frame Lines Total:"), 3, 1, 1, 1);
        dimmaDialogLayout->addWidget(linesPerFrame, 3, 2, 1, 1);


        // you have to say the variable names out loud (^_^)
        QVBoxLayout *dimDamDimmaDialogLayout = new QVBoxLayout(dim_dialog);
        dimDamDimmaDialogLayout->addLayout(dimmaDialogLayout);
        dimDamDimmaDialogLayout->addLayout(dimButtonLayout);
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
        } else {
            this->accept();
        }
    }

    void dim_accept()
    {
        s->setValue(QString("ssd_width"), horizontal->text());
        s->setValue(QString("ssd_height"), vertical->text());
        s->setValue(QString("ssd_lines_per_frame"), linesPerFrame->text());
        this->accept();
    }

private:
    std::unordered_map<std::string, source_t> source_t_name{{"SSD (ENVI)", ENVI}, {"SSD (XIO)",XIO}, {"CL", CAMERA_LINK}, {"CAMERA_LINK", CAMERA_LINK}};
    QSettings *s;
    QStringList cameraList;
    QStringList formatList;
    QListView *cameraListView;
    QStringListModel *cameraListModel;
    QCheckBox *doNotShowBox;
    QDialog *dim_dialog;
    QLineEdit *horizontal;
    QLineEdit *vertical;

    // PK 6-9-21 added 'lines per frame' input box widget.
    QLineEdit *linesPerFrame;  
};

#endif // CAMERASELECTDIALOG_H
