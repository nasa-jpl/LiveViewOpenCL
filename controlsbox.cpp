#include "controlsbox.h"

ControlsBox::ControlsBox(FrameWorker *fw, QWidget *parent) :
    QWidget(parent)
{
    frame_handler = fw;

    dirEdit = new QLineEdit();
    dirEdit->setFixedWidth(750);
    QPushButton *resetButton = new QPushButton("&Reset");
    connect(resetButton, SIGNAL(released()), this, SLOT(resetDir()));

    QHBoxLayout *cboxLayout = new QHBoxLayout;
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
