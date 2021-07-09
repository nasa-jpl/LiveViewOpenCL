#include "controlsbox.h"

ControlsBox::ControlsBox(FrameWorker *fw, QTabWidget *tw,
                         const QString &ipAddress, quint16 port,
                         QWidget *parent) :
    QWidget(parent), bit_org(fwBIL), collecting_mask(false)
{
    frame_handler = fw;
    connect(frame_handler, &FrameWorker::updateFPS,
            this, &ControlsBox::updateFPS);
    tab_handler = tw;
    connect(tab_handler, SIGNAL(currentChanged(int)),
            this, SLOT(tabChanged(int)));
    viewWidget = getCurrentTab();

    fpsLabel = new QLabel("Warning: No Frames Received");
    fpsLabel->setFixedWidth(200);
    // fpsLabel->setValidator(new QDoubleValidator(1.0, 250.0, 1, fpsLabel));

    /* connect(fpsLabel, &QLineEdit::editingFinished, this, [this]() {
        QString fpstext = fpsLabel->text();
        bool ok;
        double fpsdb = fpstext.toDouble(&ok); // Check whether value is a number
        if (ok) {
            frame_handler->setFramePeriod(1000.0 / fpsdb);
        }
    }); */

    QLabel *ipLabel = new QLabel(QString("IP Address: %1").arg(ipAddress), this);
    QLabel *portLabel = new QLabel(QString("Port Label: %1").arg(port), this);

    rangeSlider = new ctkRangeSlider(this);
    rangeSlider->setOrientation(Qt::Horizontal);
    connect(rangeSlider, &ctkRangeSlider::minimumPositionChanged, viewWidget, &LVTabApplication::setFloorPos);
    connect(rangeSlider, &ctkRangeSlider::maximumPositionChanged, viewWidget, &LVTabApplication::setCeilingPos);

    min_box = new QSpinBox(this);
    max_box = new QSpinBox(this);

    precisionBox = new QCheckBox("Precision Slider", this);
    connect(precisionBox, SIGNAL(toggled(bool)), this, SLOT(setPrecision(bool)));

    saveFileNameEdit = new QLineEdit(this);
    numFramesEdit = new QSpinBox(this);
    numFramesEdit->setMaximum(1000000);
    numAvgsEdit = new QSpinBox(this);
    numAvgsEdit->setMinimum(1);
    numAvgsEdit->setMaximum(1000000);

    //The START Button

    //let's do colors

    QPushButton *startAcquisitionButton = new QPushButton("START Acquisition", this);
    startAcquisitionButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    //startAcquisitionButton->setAutoFillBackground(true);
    //QPalette palette = startAcquisitionButton->palette();
    //palette.setColor(QPalette::Base, QColor(Qt.blue));
    //startAcquisitionButton->setPalette(palette);
    //startAcquisitionButton->show();


    connect(startAcquisitionButton, &QPushButton::clicked,
            this, &ControlsBox::acceptSave);
    connect(saveFileNameEdit, &QLineEdit::textChanged, startAcquisitionButton,
            [this]() {
        saveFileNameEdit->setToolTip(findAndReplaceFileName(saveFileNameEdit->text()));
    }); // I think this line causes an acquisition to start after you change filename. We should remove this perhaps.
    connect(startAcquisitionButton, &QPushButton::clicked, startAcquisitionButton,
            [this]() {
        saveFileNameEdit->setToolTip(findAndReplaceFileName(saveFileNameEdit->text()));
    });
    connect(this, SIGNAL(startSavingFrames(save_req_t)), frame_handler, SLOT(saveFrames(save_req_t)));

    //THE STOP BUTTON
    //TODO - make it stop when pushing this button. Nimrod Jun 2nd 2021
    QPushButton *stopAcquisitionButton = new QPushButton("STOP Acquisition", this);
    stopAcquisitionButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    connect(stopAcquisitionButton, &QPushButton::clicked,
            this, &ControlsBox::stopSaveButton);

    // end stop button

    browseButton = new QPushButton("...", this);
    // calls a function of the parent, so this button is connected to a function in the parent.

    maskButton = new QPushButton("&Collect Mask Frames", this);
    connect(maskButton, &QPushButton::released, this, &ControlsBox::collectDSFMask);
    connect(frame_handler->DSFilter, &DarkSubFilter::mask_frames_collected, this, [this](){
        this->collectDSFMask();
    });

#ifdef QT_DEBUG
    debugButton = new QPushButton("Debug", this);
    debugButton->setMaximumWidth(60);
    debugButton->setMaximumHeight(24);

    connect(debugButton, &QPushButton::clicked, this, [=](){
        emit getDebug();
    } );
#endif

    auto stdDevNSlider = new QSlider(this);
    stdDevNSlider->setOrientation(Qt::Horizontal);
    stdDevNSlider->setValue(100);
    // stdDevNSlider->setEnabled(false);
    auto stdDevNBox = new QSpinBox(this);
    stdDevNBox->setMaximum(MAX_N);
    stdDevNBox->setMinimum(1);
    // stdDevNBox->setEnabled(false);
    stdDevNBox->setValue(static_cast<int>(frame_handler->getStdDevN()));

    // TODO: convert this to new style syntax. Ambiguity of valueChanged signal causes issues.
    connect(stdDevNBox, SIGNAL(valueChanged(int)), frame_handler, SLOT(setStdDevN(int)));
    connect(stdDevNSlider, &QSlider::valueChanged, this, [stdDevNBox, stdDevNSlider]() {
        stdDevNBox->setValue((stdDevNSlider->value() / 2) + 1);
    });

    auto cboxLayout = new QGridLayout(this);
    cboxLayout->addWidget(fpsLabel, 0, 0, 1, 1);
    cboxLayout->addWidget(new QLabel("Range:", this), 0, 1, 1, 1);
    cboxLayout->addWidget(min_box, 0, 2, 1, 1);
    cboxLayout->addWidget(rangeSlider, 0, 3, 1, 2);
    cboxLayout->addWidget(max_box, 0, 5, 1, 1);
    cboxLayout->addWidget(precisionBox, 0, 9, 1, 1);
    cboxLayout->addWidget(maskButton, 0, 10, 1, 1);
    cboxLayout->addWidget(ipLabel, 1, 0, 1, 1);
    cboxLayout->addWidget(new QLabel("Save File to:", this), 1, 1, 1, 1);
    cboxLayout->addWidget(saveFileNameEdit, 1, 2, 1, 2);
    cboxLayout->addWidget(browseButton, 1, 4, 1, 1);
    cboxLayout->addWidget(startAcquisitionButton, 1, 5, 2, 2);
    cboxLayout->addWidget(stopAcquisitionButton, 3, 5, 2, 2);
#ifdef QT_DEBUG
    cboxLayout->addWidget(debugButton, 4,9,3,3);
#endif
    cboxLayout->addWidget(new QLabel("Num. Frames:", this), 1, 9, 1, 1);
    cboxLayout->addWidget(numFramesEdit, 1, 10, 1, 1);
    cboxLayout->addWidget(portLabel, 2, 0, 1, 1);
    cboxLayout->addWidget(new QLabel("Std. Dev. N:", this), 2, 1, 1, 1);
    cboxLayout->addWidget(stdDevNBox, 2, 2, 1, 1);
    cboxLayout->addWidget(stdDevNSlider, 2, 3, 1, 2);
    cboxLayout->addWidget(new QLabel("Num. Avgs:", this), 2, 9, 1, 1);
    cboxLayout->addWidget(numAvgsEdit, 2, 10, 1, 1);

    this->setLayout(cboxLayout);
    this->setMaximumHeight(150);
    tabChanged(0);

    connect(rangeSlider, &ctkRangeSlider::minimumPositionChanged, this, &ControlsBox::setMinSpin);
    connect(rangeSlider, &ctkRangeSlider::maximumPositionChanged, this, &ControlsBox::setMaxSpin);

    connect(min_box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &ControlsBox::setRangeSliderMin);
    connect(max_box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &ControlsBox::setRangeSliderMax);

    connect(min_box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [this](int new_min){
        if (new_min <= viewWidget->getCeiling())
            viewWidget->setFloor(new_min);
    });
    connect(max_box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [this](int new_max) {
        if (new_max >= viewWidget->getFloor())
            viewWidget->setCeiling(new_max);
    });
}

void ControlsBox::tabChanged(int index)
{
    Q_UNUSED( index );

    // disconnect the signals associated with the pointer to the old tab
    disconnect(rangeSlider, &ctkRangeSlider::minimumPositionChanged,
               viewWidget, &LVTabApplication::setFloorPos);
    disconnect(rangeSlider, &ctkRangeSlider::maximumPositionChanged,
               viewWidget, &LVTabApplication::setCeilingPos);

    // associate the GUI items with the pointer to the new tab
    viewWidget = getCurrentTab();
    if(!viewWidget) {
        return;
    }

    // connect the slider to the new view widget pointer
    connect(rangeSlider, &ctkRangeSlider::minimumPositionChanged,
            viewWidget, &LVTabApplication::setFloorPos);
    connect(rangeSlider, &ctkRangeSlider::maximumPositionChanged,
            viewWidget, &LVTabApplication::setCeilingPos);

    rangeSlider->blockSignals(true);
    min_box->blockSignals(true);
    max_box->blockSignals(true);
    precisionBox->setChecked(viewWidget->isPrecisionMode());
    rangeSlider->blockSignals(false);
    min_box->blockSignals(false);
    max_box->blockSignals(false);


    // update the range slider positions
    rangeSlider->setPositions(static_cast<int>(viewWidget->getFloor() / viewWidget->getDataMax() * 99.0),
                              static_cast<int>(viewWidget->getCeiling() / viewWidget->getDataMax() * 99.0));

    min_box->blockSignals(true);
    min_box->setMinimum(static_cast<int>(viewWidget->getDataMin()));
    min_box->setMaximum(static_cast<int>(viewWidget->getDataMax()));
    min_box->setValue(static_cast<int>(viewWidget->getFloor()));
    min_box->blockSignals(false);

    max_box->blockSignals(true);
    max_box->setMinimum(static_cast<int>(viewWidget->getDataMin()));
    max_box->setMaximum(static_cast<int>(viewWidget->getDataMax()));
    max_box->setValue(static_cast<int>(viewWidget->getCeiling()));
    max_box->blockSignals(false);
}

void ControlsBox::acceptSave()
{
    //if (!saveFileNameEdit->text().isEmpty() && numFramesEdit->value() != 0)

    save_req_t new_req = {bit_org,
                          findAndReplaceFileName(saveFileNameEdit->text()).toStdString(),
                          static_cast<int64_t>(numFramesEdit->value()),
                          static_cast<int64_t>(numAvgsEdit->value())};

    // TODO: emit a signal, do not call directly
    //frame_handler->saveFrames(new_req);
    emit startSavingFrames(new_req);

}

void ControlsBox::stopSaveButton()
{
    emit stopSavingFrames();
}


void ControlsBox::setMinSpin(int new_min) {
    min_box->blockSignals(true);
    min_box->setValue(static_cast<int>(new_min * viewWidget->getDataMax() / 99.0));
    min_box->blockSignals(false);
}

void ControlsBox::setMaxSpin(int new_max) {
    max_box->blockSignals(true);
    max_box->setValue(static_cast<int>(new_max * viewWidget->getDataMax() / 99.0));
    max_box->blockSignals(false);
}

void ControlsBox::setRangeSliderMin(int new_min) {
    if(new_min <= max_box->value()) {
        rangeSlider->blockSignals(true);
        rangeSlider->setMinimumPosition(static_cast<int>(new_min * 99.0 / viewWidget->getDataMax()));
        rangeSlider->blockSignals(false);
    }
}

void ControlsBox::setRangeSliderMax(int new_max) {
    if(new_max >= min_box->value()) {
        rangeSlider->blockSignals(true);
        rangeSlider->setMaximumPosition(static_cast<int>(new_max * 99.0 / viewWidget->getDataMax()));
        rangeSlider->blockSignals(false);
    }
}

void ControlsBox::setPrecision(bool isPrecise)
{
    viewWidget->setPrecision(isPrecise);

    if (isPrecise) {
        // Set the minimum position out of 100, so the slider can go as low as -dataMax
        rangeSlider->setMinimum(-100);
    } else {
        rangeSlider->setMinimum(0);
    }

    // update the range slider positions
    rangeSlider->setPositions(static_cast<int>(viewWidget->getFloor() / viewWidget->getDataMax() * 99.0),
                              static_cast<int>(viewWidget->getCeiling() / viewWidget->getDataMax() * 99.0));

    min_box->setMinimum(static_cast<int>(viewWidget->getDataMin()));
    min_box->setMaximum(static_cast<int>(viewWidget->getDataMax()));
    min_box->setValue(static_cast<int>(viewWidget->getFloor()));

    max_box->setMinimum(static_cast<int>(viewWidget->getDataMin()));
    max_box->setMaximum(static_cast<int>(viewWidget->getDataMax()));
    max_box->setValue(static_cast<int>(viewWidget->getCeiling()));
}

void ControlsBox::collectDSFMask()
{
    collecting_mask = !collecting_mask;
    if (collecting_mask) {
        maskButton->setText("&Stop Collecting Mask");
        frame_handler->collectMask();
    } else {
        maskButton->setText("&Collect Mask Frames");
        frame_handler->stopCollectingMask();
    }
}

void ControlsBox::updateFPS(double frameRate)
{
    if (static_cast<int>(frameRate) == -1) {
        fpsLabel->setText(QString("Warning: No Frames Received"));
    } else {
        fpsLabel->setText(QString("FPS @ backend: %1")
                          .arg(QString::number(frameRate, 'f', 1)));
    }
}

QString ControlsBox::findAndReplaceFileName(const QString& fileName)
{
    if (fileName != prevFileName) {
        fileNumber = 0;
        prevFileName = fileName;
    }

    QString outStr = fileName;
    QString dateStr = "%1_%2_%3";
    QDate date = QDateTime::currentDateTime().date();

    outStr.replace("%t", QString::number(QDateTime::currentDateTime().toSecsSinceEpoch()));
    outStr.replace("%n", QString::number(fileNumber++));
    outStr.replace("%d", dateStr.arg(QString::number(date.year()),
                                     QString::number(date.month()),
                                     QString::number(date.day())));

    return outStr;
}

LVTabApplication* ControlsBox::getCurrentTab()
{
    return qobject_cast<LVTabApplication*>(tab_handler->widget(
                                               tab_handler->currentIndex()));
}
