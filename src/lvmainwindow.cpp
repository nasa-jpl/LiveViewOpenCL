#include "lvmainwindow.h"

LVMainWindow::LVMainWindow(QWidget *parent)
    : QMainWindow(parent)
{   
    // Hardcoded default window size
    this->resize(1440, 900);

    QPixmap icon_pixmap(":images/icon.png");
    this->setWindowIcon(QIcon(icon_pixmap));
    this->setWindowTitle("LiveView 4.0");

    // Load the worker thread
    workerThread = new QThread;
    fw = new FrameWorker(workerThread);
    fw->moveToThread(workerThread);
    // Reserve proper take object error handling for later
    connect(fw, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(workerThread, SIGNAL(started()), fw, SLOT(captureFrames()));
    connect(fw, SIGNAL(finished()), workerThread, SLOT(quit()));
    connect(fw, SIGNAL(finished()), fw, SLOT(deleteLater()));
    connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));

    connect(fw, &FrameWorker::startSaving, this, [&](){
        saveAct->setEnabled(false);
        saveAsAct->setEnabled(false);
    });
    connect(fw, &FrameWorker::doneSaving, this, [&](){
        saveAct->setEnabled(true);
        saveAsAct->setEnabled(true);
    });

    if (fw->running()) {
        workerThread->start();
        DSLoop = QtConcurrent::run(fw, &FrameWorker::captureDSFrames);
        SDLoop = QtConcurrent::run(fw, &FrameWorker::captureSDFrames);
    }

    QWidget* mainWidget = new QWidget(this);
    tab_widget = new QTabWidget(this);

    raw_display = new frameview_widget(fw, BASE);
    dsf_display = new frameview_widget(fw, DSF);
    sdv_display = new frameview_widget(fw, STD_DEV);
    hst_display = new histogram_widget(fw);
    spec_display = new line_widget(fw, SPECTRAL_PROFILE);
    spec_mean_display = new line_widget(fw, SPECTRAL_MEAN);
    spat_display = new line_widget(fw, SPATIAL_PROFILE);
    spat_mean_display = new line_widget(fw, SPATIAL_MEAN);
    fft_display = new fft_widget(fw);

    // Set these two to be in the precision slider by default
    dsf_display->setPrecision(true);
    sdv_display->setPrecision(true);

    tab_widget->addTab(raw_display, QString("Live View"));
    tab_widget->addTab(dsf_display, QString("Dark Subtraction"));
    tab_widget->addTab(sdv_display, QString("Standard Deviation"));
    tab_widget->addTab(hst_display, QString("Histogram"));
    tab_widget->addTab(spec_display, QString("Spectral Profile"));
    tab_widget->addTab(spec_mean_display, QString("Spectral Mean"));
    tab_widget->addTab(spat_display, QString("Spatial Profile"));
    tab_widget->addTab(spat_mean_display, QString("Spatial Mean"));
    tab_widget->addTab(fft_display, QString("FFT of Plane Mean"));

    /*
     * It's pretty bizarre to send the tab widget into the ControlsBox, but the reference is
     * preserved so that the ControlsBox GUI elements will control the current tab in context.
     */
    cbox = new ControlsBox(fw, tab_widget);

    QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->addWidget(tab_widget);
    mainLayout->addWidget(cbox);

    mainWidget->setLayout(mainLayout);
    this->setCentralWidget(mainWidget);

    createActions();
    createMenus();

    server = new SaveServer(this);
    connect(server, &SaveServer::startSavingRemote, fw, &FrameWorker::captureFramesRemote);

    compDialog = new ComputeDevDialog(fw->STDFilter->getDeviceList());
    connect(compDialog, &ComputeDevDialog::device_changed, this, &LVMainWindow::change_compute_device);

    dsfDialog = new DSFPrefDialog();
}

LVMainWindow::~LVMainWindow()
{
    fw->stop();
    DSLoop.waitForFinished();
    SDLoop.waitForFinished();
}

void LVMainWindow::errorString(const QString &errstr)
{
    qWarning() << errstr;
}

void LVMainWindow::createActions()
{
    openAct = new QAction("&Open", this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip("Open an existing data set");
    connect(openAct, &QAction::triggered, this, &LVMainWindow::open);

    saveAct = new QAction("&Save", this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip("Save frames to file");
    connect(saveAct, &QAction::triggered, this, &LVMainWindow::save);

    saveAsAct = new QAction("Save &As...", this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip("Select a location to save frames");
    connect(saveAsAct, &QAction::triggered, this, &LVMainWindow::saveAs);

    resetAct = new QAction("&Reset", this);
    resetAct->setShortcuts(QKeySequence::Refresh);
    resetAct->setStatusTip("Restart the data stream");
    connect(resetAct, &QAction::triggered, this, &LVMainWindow::reset);

    exitAct = new QAction("E&xit", this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip("Exit LiveView");
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    compAct = new QAction("Change Compute Device...", this);
    compAct->setStatusTip("Use a different computing type for OpenCL calculations.");
    connect(compAct, &QAction::triggered, this, &LVMainWindow::show_deviceModelView);

    dsfAct = new QAction("Dark Subtraction", this);
    dsfAct->setShortcut(QKeySequence::Deselect); // This specifies the Ctrl+D key combo.
    dsfAct->setStatusTip("Modify settings when collecting dark subtraction frames.");
    connect(dsfAct, &QAction::triggered, this, &LVMainWindow::show_dsfModelView);

}

void LVMainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    // saveAct->setEnabled(false);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(resetAct);
    // These two items will not appear in MacOS because they are handled automatically by the
    // application menu.
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    prefMenu = menuBar()->addMenu("&Computation");
    prefMenu->addAction(compAct);
    prefMenu->addAction(dsfAct);
}

#ifndef QT_NO_CONTEXTMENU
void LVMainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(openAct);
    menu.addAction(saveAct);
    menu.addAction(saveAsAct);
    menu.addAction(resetAct);
    menu.addAction(exitAct);
    menu.exec(event->globalPos());

    QMenu compMenu(this);
    compMenu.addAction(compAct);
    compMenu.addAction(dsfAct);
    compMenu.exec(event->globalPos());
}
#endif // QT_NO_CONTEXTMENU

void LVMainWindow::open()
{
    default_dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString temp_dir = QFileDialog::getExistingDirectory(this, "Open Data Directory",
                           default_dir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!temp_dir.isEmpty()) {
        source_dir = temp_dir;
        fw->resetDir(source_dir.toLatin1().data());
    }
}

void LVMainWindow::save()
{
    if (save_filename.isEmpty()) {
        saveAs();
        if (save_filename.isEmpty()) {
            return;
        }
    } else {
        fw->captureFramesRemote(save_filename, (quint64)10, (quint64)1);
    }
}

void LVMainWindow::saveAs()
{
    save_filename = QFileDialog::getSaveFileName(this, "Save Raw Frames",
                        default_dir, "Raw Camera Frames (*.raw);;All files (*.*)");
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    if (!save_filename.isEmpty()) {
        // saveAct->setEnabled(true);
        fw->captureFramesRemote(save_filename, (quint64)10, (quint64)1);
    }
}

void LVMainWindow::reset()
{
    fw->resetDir(source_dir.toLatin1().data());
}

void LVMainWindow::show_deviceModelView()
{
    compDialog->show();
}

void LVMainWindow::show_dsfModelView()
{
    dsfDialog->show();
}

void LVMainWindow::change_compute_device(QString dev_name)
{
    fw->STDFilter->change_device(dev_name);
}
