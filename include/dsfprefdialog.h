#ifndef DSFPREFDIALOG_H
#define DSFPREFDIALOG_H

#include <QBoxLayout>
#include <QCheckBox>
#include <QDebug>
#include <QDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QMessageBox>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QStandardPaths>

class DSFPrefDialog : public QDialog
{
    Q_OBJECT

public:
    DSFPrefDialog() : mask_file(""), avgd_frames(1), open_mode(true)
    {
        this->setWindowTitle("Set Dark Subtraction Mask Preferences");
        this->setModal(true);

        QRadioButton *openChoice = new QRadioButton("&Open an Existing Mask", this);
        QRadioButton *saveChoice = new QRadioButton("&Save a New Mask", this);
        connect(openChoice, &QRadioButton::clicked, this, [this]() {
            useFixedFrames->setChecked(false);
            useFixedFrames->setEnabled(false);
            open_mode = true;
        });
        connect(saveChoice, &QRadioButton::clicked, this, [this]() {
            qDebug() << "这是来自 Save Mode Button 的测试。";
            useFixedFrames->setEnabled(true);
            open_mode = false;
        });

        QHBoxLayout *radioLayout = new QHBoxLayout;
        radioLayout->addWidget(openChoice);
        radioLayout->addWidget(saveChoice);

        fileEdit = new QLineEdit(this);

        QPushButton *browseButton = new QPushButton("&Browse...", this);
        connect(browseButton, &QPushButton::clicked, this, &DSFPrefDialog::show_fileDialog);

        useFixedFrames = new QCheckBox("Use a fixed number of frames when recording Dark Subtraction mask", this);

        QHBoxLayout *checkLayout = new QHBoxLayout;
        checkLayout->addWidget(useFixedFrames);

        avgdFramesBox = new QSpinBox(this);
        avgdFramesBox->setMinimum(1);
        avgdFramesBox->setValue(avgd_frames);
        maskLabel = new QLabel("Num. Frames to Avg. in Mask:", this);

        QHBoxLayout *avgLayout = new QHBoxLayout;
        avgLayout->addWidget(maskLabel);
        avgLayout->addWidget(avgdFramesBox);
        maskLabel->setVisible(false);
        avgdFramesBox->setVisible(false);

        connect(useFixedFrames, &QCheckBox::toggled, this, [this](bool toggled)
        {
            qDebug() << "这个错了吗？";
            avgdFramesBox->setVisible(toggled);
            maskLabel->setVisible(toggled);
        });

        QPushButton *applyButton = new QPushButton("&Apply", this);
        QPushButton *okButton = new QPushButton("&Ok", this);
        QPushButton *cancelButton = new QPushButton("&Cancel", this);
        connect(applyButton, &QPushButton::clicked, this, &DSFPrefDialog::applyMask);
        connect(okButton, &QPushButton::clicked, this, &DSFPrefDialog::okButtonPressed);
        connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

        QHBoxLayout *buttonLayout = new QHBoxLayout;
        buttonLayout->addWidget(applyButton);
        buttonLayout->addWidget(okButton);
        buttonLayout->addWidget(cancelButton);

        QHBoxLayout *fileEditLayout = new QHBoxLayout;
        fileEditLayout->addWidget(new QLabel("Mask File:"));
        fileEditLayout->addWidget(fileEdit);
        fileEditLayout->addWidget(browseButton);

        QVBoxLayout *dialogLayout = new QVBoxLayout(this);
        dialogLayout->addLayout(radioLayout);
        dialogLayout->addLayout(fileEditLayout);
        dialogLayout->addLayout(checkLayout);
        dialogLayout->addLayout(avgLayout);
        dialogLayout->addLayout(buttonLayout);

        openChoice->click();
    }

    const QString& getMaskFile() { return mask_file; }
    const quint64& getAvgdFrames() { return avgd_frames; }

signals:
    void applyMaskFromFile(QString);
    void changeMaskSettings();

private slots:
    void okButtonPressed()
    {
        applyMask();
        if (useFixedFrames->isChecked()) {
            avgd_frames = avgdFramesBox->value();
        }
        this->accept();
    }

    void show_fileDialog()
    {
        QString default_dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        QString temp_file = "";
        if (open_mode) {
            temp_file = QFileDialog::getOpenFileName(this, "Open Mask File",
                                                     default_dir, "All files (*.*)");
        } else {
            temp_file = QFileDialog::getSaveFileName(this, "Save Mask File",
                                                     default_dir, "All files (*.*)");
        }

        if (!temp_file.isEmpty() && temp_file != mask_file) {
            fileEdit->setText(temp_file);
        }
    }
    void applyMask()
    {
        mask_file = fileEdit->text();
        if (QFileInfo::exists(mask_file) && QFileInfo(mask_file).isFile()) {
            emit applyMaskFromFile(mask_file);
        }
    }

protected slots:
    virtual void showEvent(QShowEvent *e) {
        Q_UNUSED(e);
        fileEdit->setText(mask_file);
    }

private:
    QString mask_file;
    quint64 avgd_frames;
    bool open_mode;
    QLineEdit *fileEdit;
    QCheckBox *useFixedFrames;
    QSpinBox *avgdFramesBox;
    QLabel *maskLabel;
};

#endif // DSFPREFDIALOG_H
