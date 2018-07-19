#ifndef COMPUTEDEVDIALOG_H
#define COMPUTEDEVDIALOG_H

#include <QDialog>
#include <QStringList>
#include <QLabel>
#include <QListView>
#include <QStringListModel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>


class ComputeDevDialog : public QDialog
{
    Q_OBJECT

public:
    ComputeDevDialog(QStringList device_list) : deviceList(device_list)
    {
        this->setWindowTitle("Select OpenCL Device");
        this->setModal(true);

        deviceListView = new QListView(this);
        deviceListModel = new QStringListModel(this);
        deviceListModel->setStringList(deviceList);
        deviceListView->setModel(deviceListModel);
        deviceListView->setSelectionMode(QAbstractItemView::SingleSelection);

        QPushButton *okButton = new QPushButton("&Ok", this);
        QPushButton *cancelButton = new QPushButton("&Cancel", this);
        connect(okButton, &QPushButton::clicked, this, &ComputeDevDialog::okButtonPressed);
        connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

        QHBoxLayout *buttonLayout = new QHBoxLayout;
        buttonLayout->addWidget(okButton);
        buttonLayout->addWidget(cancelButton);
        QVBoxLayout *dialogLayout = new QVBoxLayout(this);
        dialogLayout->addWidget(new QLabel("Select a computing device to use for Standard Deviviation computation:"));
        dialogLayout->addWidget(deviceListView);
        dialogLayout->addLayout(buttonLayout);
    }

    QStringList deviceList;
    QListView *deviceListView;
    QStringListModel *deviceListModel;

signals:
    void device_changed(QString);

private slots:
    void okButtonPressed() {
        QString device_name("");
        if (deviceListView->selectionModel()->selectedIndexes().size() > 0) {
            QVariant device_data = deviceListModel->data(deviceListView->selectionModel()->selectedIndexes()[0]);
            device_name = device_data.toString();
        }
        emit device_changed(device_name);
        this->accept();
    }

};

#endif // COMPUTEDEVDIALOG_H
