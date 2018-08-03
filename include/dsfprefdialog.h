#ifndef DSFPREFDIALOG_H
#define DSFPREFDIALOG_H

#include <QDebug>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QBoxLayout>
#include <QFileDialog>
#include <QFileInfo>
#include <QStandardPaths>
#include <QMessageBox>

class DSFPrefDialog : public QDialog
{
    Q_OBJECT

public:
    DSFPrefDialog()
    {
        this->setWindowTitle("Set Dark Subtraction Mask Preferences");
        this->setModal(true);

        fileEdit = new QLineEdit(this);

        QPushButton *browseButton = new QPushButton("&Browse...");
        connect(browseButton, &QPushButton::clicked, this, &DSFPrefDialog::show_fileDialog);

        QCheckBox *useFixedFrames = new QCheckBox("Use a fixed number of frames when recording Dark Subtraction mask", this);

        QHBoxLayout *checkLayout = new QHBoxLayout;
        checkLayout->addWidget(useFixedFrames);

        QSpinBox *avgdFramesBox = new QSpinBox(this);
        avgdFramesBox->setMinimum(0);
        QLabel *maskLabel = new QLabel("Num. Frames to Avg. in Mask:");

        QHBoxLayout *avgLayout = new QHBoxLayout();
        avgLayout->addWidget(maskLabel);
        avgLayout->addWidget(avgdFramesBox);
        maskLabel->setVisible(false);
        avgdFramesBox->setVisible(false);

        connect(useFixedFrames, &QCheckBox::toggled, this, [avgdFramesBox, maskLabel](bool toggled)
        {
            avgdFramesBox->setVisible(toggled);
            maskLabel->setVisible(toggled);
        });

        QPushButton *applyButton = new QPushButton("&Apply", this);
        QPushButton *okButton = new QPushButton("&Ok", this);
        QPushButton *cancelButton = new QPushButton("&Cancel", this);
        connect(applyButton, &QPushButton::clicked, this, &DSFPrefDialog::applyMask);
        connect(okButton, &QPushButton::clicked, this, [this]()
        {
            applyMask();
            this->accept();
        });
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
        dialogLayout->addLayout(fileEditLayout);
        dialogLayout->addLayout(checkLayout);
        dialogLayout->addLayout(avgLayout);
        dialogLayout->addLayout(buttonLayout);
    }

    const QString& getMaskFile() { return mask_file; }

signals:
    void applyMaskFromFile(QString);

private slots:
    void okButtonPressed()
    {
        this->accept();
    }
    void show_fileDialog()
    {
        QString default_dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        QString temp_file = QFileDialog::getSaveFileName(this, "Save Mask File", default_dir);
        if (!temp_file.isEmpty() && temp_file != mask_file) {
            mask_file = temp_file;
            fileEdit->setText(mask_file);
        }
    }
    void applyMask()
    {
        if (QFileInfo::exists(mask_file) && QFileInfo(mask_file).isFile()) {
            emit applyMaskFromFile(mask_file);
        } else if (!mask_file.isEmpty()) {
            QMessageBox::warning(this, "File not found", QString("Unable to find mask file: %1").arg(mask_file),
                                 QMessageBox::Ok);
        }
    }

private:
    QString mask_file;
    QLineEdit *fileEdit;
};

#endif // DSFPREFDIALOG_H
