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
    tab_widget = new QTabWidget();

    raw_display = new frameview_widget(BASE, fw);
    dsf_display = new frameview_widget(DSF, fw);
    sdv_display = new frameview_widget(STD_DEV, fw);

    tab_widget->addTab(raw_display, QString("Live View"));
    tab_widget->addTab(dsf_display, QString("Dark Subtraction"));
    tab_widget->addTab(sdv_display, QString("Standard Deviation"));

    /*
     * It's pretty bizarre to send the tab widget into the ControlsBox, but the reference is
     * preserved so that the ControlsBox GUI elements will control the current tab in context.
     */
    cbox = new ControlsBox(fw, tab_widget);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addWidget(tab_widget);
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
