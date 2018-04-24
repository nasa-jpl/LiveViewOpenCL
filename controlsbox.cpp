#include "controlsbox.h"

ControlsBox::ControlsBox(FrameWorker *fw, QWidget *parent) :
    QWidget(parent), collecting_mask(false)
{
    frame_handler = fw;

    dirEdit = new QLineEdit();
    dirEdit->setFixedWidth(750);
    QPushButton *resetButton = new QPushButton("&Reset");
    connect(resetButton, SIGNAL(released()), this, SLOT(resetDir()));

    maskButton = new QPushButton("&Collect Mask Frames");
    connect(maskButton, SIGNAL(released()), this, SLOT(collectDSFMask()));

    QHBoxLayout *cboxLayout = new QHBoxLayout;
    cboxLayout->addWidget(maskButton);
    cboxLayout->addWidget(new QLabel("Input directory:"));
    cboxLayout->addWidget(dirEdit);
    cboxLayout->addWidget(resetButton);
    this->setLayout(cboxLayout);
    this->setMaximumHeight(100);
}

ControlsBox::~ControlsBox()
{
}

void ControlsBox::resetDir()
{
    QString dirname = dirEdit->text();
    QByteArray ba = dirname.toLatin1();
    const char *dname = ba.data();
    frame_handler->resetDir(dname);
}

void ControlsBox::collectDSFMask()
{
    collecting_mask = !collecting_mask;
    if (collecting_mask) {
        maskButton->setText("&Stop Collecting Mask");
        frame_handler->DSFilter->start_mask_collection();
    } else {
        maskButton->setText("&Collect Mask Frames");
        frame_handler->DSFilter->finish_mask_collection();
    }

}
