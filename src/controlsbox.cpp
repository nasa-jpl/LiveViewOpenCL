#include "controlsbox.h"

ControlsBox::ControlsBox(FrameWorker *fw, QTabWidget *tw,
                         QString ipAddress, quint16 port, QWidget *parent) :
    QWidget(parent), collecting_mask(false)
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
    QPushButton *saveFramesButton = new QPushButton("Save Frames", this);
    saveFramesButton->setIcon(style()->standardIcon(QStyle::SP_DriveHDIcon));
    connect(saveFramesButton, &QPushButton::clicked, this, [this]() {
        const QString &fileName = saveFileNameEdit->text();
        const int &numFrames = numFramesEdit->value();
        const int &numAvgs = numAvgsEdit->value();
        if (!fileName.isEmpty() && numFrames > 0) {
            frame_handler->saveFrames(fileName.toStdString(),
                                      static_cast<uint64_t>(numFrames),
                                      static_cast<uint64_t>(numAvgs));
        }
    });

    browseButton = new QPushButton("...", this);
    // calls a function of the parent, so this button is connected to a function in the parent.

    maskButton = new QPushButton("&Collect Mask Frames", this);
    connect(maskButton, &QPushButton::released, this, &ControlsBox::collectDSFMask);
    connect(frame_handler->DSFilter, &DarkSubFilter::mask_frames_collected, this, [this](){
        this->collectDSFMask();
    });

    QSlider *stdDevNSlider = new QSlider(this);
    stdDevNSlider->setOrientation(Qt::Horizontal);
    stdDevNSlider->setValue(100);
    // stdDevNSlider->setEnabled(false);
    QSpinBox *stdDevNBox = new QSpinBox(this);
    stdDevNBox->setMaximum(MAX_N);
    stdDevNBox->setMinimum(1);
    // stdDevNBox->setEnabled(false);
    stdDevNBox->setValue(static_cast<int>(frame_handler->getStdDevN()));

    // TODO: convert this to new style syntax. Ambiguity of valueChanged signal causes issues.
    connect(stdDevNBox, SIGNAL(valueChanged(int)), frame_handler, SLOT(setStdDevN(int)));
    connect(stdDevNSlider, &QSlider::valueChanged, this, [stdDevNBox, stdDevNSlider]() {
        stdDevNBox->setValue((stdDevNSlider->value() / 2) + 1);
    });

    QGridLayout *cboxLayout = new QGridLayout(this);
    cboxLayout->addWidget(fpsLabel, 0, 0, 1, 1);
    cboxLayout->addWidget(new QLabel("Range:", this), 0, 1, 1, 1);
    cboxLayout->addWidget(min_box, 0, 2, 1, 1);
    cboxLayout->addWidget(rangeSlider, 0, 3, 1, 5);
    cboxLayout->addWidget(max_box, 0, 8, 1, 1);
    cboxLayout->addWidget(precisionBox, 0, 9, 1, 1);
    cboxLayout->addWidget(maskButton, 0, 10, 1, 1);
    cboxLayout->addWidget(ipLabel, 1, 0, 1, 1);
    cboxLayout->addWidget(new QLabel("Save File to:", this), 1, 1, 1, 1);
    cboxLayout->addWidget(saveFileNameEdit, 1, 2, 1, 5);
    cboxLayout->addWidget(browseButton, 1, 7, 1, 1);
    cboxLayout->addWidget(saveFramesButton, 1, 8, 1, 1);
    cboxLayout->addWidget(new QLabel("Num. Frames:", this), 1, 9, 1, 1);
    cboxLayout->addWidget(numFramesEdit, 1, 10, 1, 1);
    cboxLayout->addWidget(portLabel, 2, 0, 1, 1);
    cboxLayout->addWidget(new QLabel("Std. Dev. N:", this), 2, 1, 1, 1);
    cboxLayout->addWidget(stdDevNBox, 2, 2, 1, 1);
    cboxLayout->addWidget(stdDevNSlider, 2, 3, 1, 5);
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

ControlsBox::~ControlsBox() {}

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
    qDebug() << "Max box:" << max_box->value();
    max_box->blockSignals(false);
}

void ControlsBox::acceptSave()
{
    if (saveFileNameEdit->text().isEmpty() || numFramesEdit->value() == 0) {
        return;
    } else {
        frame_handler->saveFrames(saveFileNameEdit->text().toStdString(),
                                  static_cast<uint64_t>(numFramesEdit->value()),
                                  static_cast<uint64_t>(numAvgsEdit->value()));
    }
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

LVTabApplication* ControlsBox::getCurrentTab()
{
    return qobject_cast<LVTabApplication*>(tab_handler->widget(
                                               tab_handler->currentIndex()));
}
