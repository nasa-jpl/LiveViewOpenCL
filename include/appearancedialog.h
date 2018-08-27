#ifndef APPEARANCEDIALOG_H
#define APPEARANCEDIALOG_H

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
#include <qcustomplot.h>

class AppearanceDialog : public QDialog
{
    Q_OBJECT

public:
    AppearanceDialog(QSettings *set) : s(set)
    {
        QMetaEnum qme = QMetaEnum::fromType<QCPColorGradient::GradientPreset>();
        this->cameraList = QStringList();
        for (int i = 0; i < qme.keyCount(); ++i) {
            this->cameraList << qme.key(i);
        }

        this->setWindowTitle("Change Appearance");

        cameraListView = new QListView(this);
        cameraListModel = new QStringListModel(this);
        cameraListModel->setStringList(cameraList);
        cameraListView->setModel(cameraListModel);
        cameraListView->setSelectionMode(QAbstractItemView::SingleSelection);
        cameraListView->setCurrentIndex(cameraListModel->index(0));

        QPushButton *okButton = new QPushButton("&Ok", this);
        connect(okButton, &QPushButton::clicked, this,
                &AppearanceDialog::okButtonPressed);

        QPushButton *cancelButton = new QPushButton("&Cancel", this);
        connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

        QHBoxLayout *buttonLayout = new QHBoxLayout;
        buttonLayout->addWidget(okButton);
        buttonLayout->addWidget(cancelButton);

        QVBoxLayout *dialogLayout = new QVBoxLayout(this);
        dialogLayout->addWidget(new QLabel("Select Color Gradient"));
        dialogLayout->addWidget(cameraListView);
        dialogLayout->addLayout(buttonLayout);
    }

    QSettings *s;
    QStringList cameraList;
    QListView *cameraListView;
    QStringListModel *cameraListModel;
    QCheckBox *darkModeBox;

private slots:
    void okButtonPressed()
    {
        s->setValue(QString("gradient"),
                    cameraListView->selectionModel()->currentIndex().row());
        //s->setValue(QString("gradient"), 4);
        this->accept();
    }
};

#endif // APPEARANCEDIALOG_H
