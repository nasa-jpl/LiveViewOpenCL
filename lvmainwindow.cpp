#include "lvmainwindow.h"

LVMainWindow::LVMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Hardcoded default window size
    this->resize(1440, 900);

    // Load the worker thread
    FrameThread* workerThread = new FrameThread();
    fw = new FrameWorker(workerThread);
    fw->moveToThread(workerThread);
    // Reserve proper take object error handling for later
    QObject::connect(fw, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    QObject::connect(workerThread, SIGNAL(started()), fw, SLOT(captureFrames()));
    QObject::connect(fw, SIGNAL(finished()), workerThread, SLOT(quit()));
    QObject::connect(fw, SIGNAL(finished()), fw, SLOT(deleteLater()));
    QObject::connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));

    if (fw->running()) {
        workerThread->start();
    }

    QWidget* mainWidget = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout();
    raw_display = new frameview_widget(BASE, fw);
    cbox = new ControlsBox(fw);
    mainLayout->addWidget(raw_display);
    mainLayout->addWidget(cbox);
    mainWidget->setLayout(mainLayout);
    this->setCentralWidget(mainWidget);

}

LVMainWindow::~LVMainWindow()
{
    fw->stop();
}

void LVMainWindow::errorString(const QString &errstr)
{
    qWarning() << errstr;
}
