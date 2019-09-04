#ifndef FRAMERATEDIALOG_H
#define FRAMERATEDIALOG_H

#include <QBoxLayout>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>


class FrameRateDialog : public QDialog
{
    Q_OBJECT

public:
    FrameRateDialog(int framerate)
    {
        this->setWindowTitle("Change Playback Framerate");

        QPushButton *applyButton = new QPushButton("&Apply", this);
        connect(applyButton, &QPushButton::clicked, this, &FrameRateDialog::applyFramerate);
        QPushButton *cancelButton = new QPushButton("&Cancel", this);
        connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

        fpsEdit = new QSpinBox(this);
        fpsEdit->setValue(framerate);

        topLabel = new QLabel(QString("Current target playback framerate: %1 fps").arg(framerate));

        QVBoxLayout *dialogLayout = new QVBoxLayout(this);
        dialogLayout->addWidget(topLabel);
        QHBoxLayout *middleRow = new QHBoxLayout();
        middleRow->addWidget(new QLabel("New Framerate (fps): "));
        middleRow->addWidget(fpsEdit);
        dialogLayout->addLayout(middleRow);
        QHBoxLayout *bottomButtons = new QHBoxLayout();
        bottomButtons->addWidget(applyButton);
        bottomButtons->addWidget(cancelButton);
        dialogLayout->addLayout(bottomButtons);
    }

    QSpinBox *fpsEdit;
    QLabel *topLabel;

signals:
    void framerate_changed(int);

private slots:
    void applyFramerate() {
        topLabel->setText(QString("Current target playback framerate: %1 fps").arg(fpsEdit->value()));
        emit framerate_changed(fpsEdit->value());
        this->accept();
    }

};

#endif // FRAMERATEDIALOG_H
