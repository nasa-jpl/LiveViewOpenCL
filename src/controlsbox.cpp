#include "controlsbox.h"


frameLineControlData   frameLineDisplayInfo;
QMutex frameLineDisplayInfoMutex;


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
    connect(saveFramesButton, &QPushButton::clicked,
            this, &ControlsBox::acceptSave);
    connect(saveFileNameEdit, &QLineEdit::textChanged, saveFramesButton,
            [this]() {
        saveFileNameEdit->setToolTip(findAndReplaceFileName(saveFileNameEdit->text()));
    });
    connect(saveFramesButton, &QPushButton::clicked, saveFramesButton,
            [this]() {
        saveFileNameEdit->setToolTip(findAndReplaceFileName(saveFileNameEdit->text()));
    });

    browseButton = new QPushButton("...", this);
    // calls a function of the parent, so this button is connected to a function in the parent.

    maskButton = new QPushButton("&Collect Mask Frames", this);
    connect(maskButton, &QPushButton::released, this, &ControlsBox::collectDSFMask);
    connect(frame_handler->DSFilter, &DarkSubFilter::mask_frames_collected, this, [this](){
        this->collectDSFMask();
    });

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



    //
    // PK EMITFPIED-331_v2 Frame Control enhancement
    // 
    // add Frame Control buttons: Rewind, Play/Stop, Forward
    frameWorkerParent = fw;

    const QSize btnSize = QSize(35, 35);
    frameControl_prevButton = new QToolButton( this );
    frameControl_prevButton->setFixedSize( btnSize );  // 3-3-21 increase button size
    frameControl_prevButton->setIcon( style()->standardIcon(QStyle::SP_MediaSkipBackward) );
    frameControl_prevButton->setStyleSheet( "QToolButton { background-color: white; }" );

    connect(frameControl_prevButton, &QAbstractButton::clicked, this, &ControlsBox::frameControlPrevButtonClicked);

    frameControl_nextButton = new QToolButton( this );
    frameControl_nextButton->setFixedSize( btnSize );  // 3-3-21 increase button size
    frameControl_nextButton->setIcon( style()->standardIcon(QStyle::SP_MediaSkipForward) );
    frameControl_nextButton->setStyleSheet( "QToolButton { background-color: white; }" );

    connect(frameControl_nextButton, &QAbstractButton::clicked, this, &ControlsBox::frameControlNextButtonClicked);

    
    //
    // Note, initially, there is only a STOP button as the
    // image frames are being played/displayed.  When the
    // STOP is pressed, it becomes a PLAY button !!
    frameControl_stopButton = new QToolButton( this );
    frameControl_stopButton->setFixedSize( btnSize );  // 3-3-21 increase button size
    frameControl_stopButton->setIcon( style()->standardIcon(QStyle::SP_MediaStop) );
    frameControl_stopButton->setStyleSheet( "QToolButton { background-color: white; }" );

    connect(frameControl_stopButton, &QAbstractButton::clicked, this, &ControlsBox::frameControlStopButtonClicked);

    frameControlBar = new QToolBar( this );
    frameControlBar->addWidget( frameControl_prevButton );
    frameControlBar->addWidget( frameControl_stopButton );
    frameControlBar->addWidget( frameControl_nextButton );

    //
    // 3-15-21 frame control status widgets enhancement
    enum cboxLayout_Y_POS{
        cboxLayout_TOP_LINE = 0,
        cboxLayout_Line_1 = cboxLayout_TOP_LINE + 1,
        cboxLayout_Line_2 = cboxLayout_Line_1 + 1,
        cboxLayout_Line_3 = cboxLayout_Line_2 + 1,
        cboxLayout_Line_4 = cboxLayout_Line_3 + 1
    };

    auto cboxLayout = new QGridLayout(this);

    // PK 3-4-21 added image-line-control ...
    //
    // add a top line for frame line control widgets ...
    //
    // By default, when LiveView starts up, LiveView tab is the current
    // tab that displays frame lines.  So, get the current frameview_widget
    // pointer to set up the frame line control widgets.
    // 
    frameDisplay   = qobject_cast<frameview_widget *> (getCurrentTab());

    frameFilename  = new QLabel("Image Filename: ");

    frameLineNo    = new QLabel("Line #: ");
    frameLineCount = new QLabel("Line Count: ");
    frameCollectionID = new QLabel("Collection ID: ");
    frameLineControl  = new QCheckBox("Line Control", this);
    
    //
    // connect the widget with :
    // Signal Emitter, Signal name,
    // Signal Receiver, Signal processing function
    connect( frameDisplay, &frameview_widget::updateFrameFilename,
             this, &ControlsBox::updateFrameFilename );

    connect( frameDisplay, &frameview_widget::updateFrameLineInfo,
             this, &ControlsBox::updateFrameLineInfo );

    connect( frameLineControl, SIGNAL(toggled(bool)), this, SLOT(frameLineControlChecked(bool)));


    //
    // 3-15-21 frame control status widgets enhancement ...
    frameControlStatus        = new QLabel("Frame Control:  OFF");
    frameLineControlStatus    = new QLabel("Frame Line Control:  OFF");

    cboxLayout->addWidget(frameControlStatus,       cboxLayout_TOP_LINE, 3, 1, 3);
    cboxLayout->addWidget(frameLineControlStatus,   cboxLayout_TOP_LINE, 6, 1, 1);
    // ... 3-15-21 frame control status widgets enhancement 
    


    cboxLayout->addWidget(frameFilename,     cboxLayout_Line_1, 0, 1, 3);
    cboxLayout->addWidget(frameLineNo,       cboxLayout_Line_1, 3, 1, 2);
    cboxLayout->addWidget(frameLineCount,    cboxLayout_Line_1, 5, 1, 2);
    cboxLayout->addWidget(frameCollectionID, cboxLayout_Line_1, 7, 1, 2);  // 4 - this line up perfectly 
    cboxLayout->addWidget(frameLineControl,  cboxLayout_Line_1, 9, 1, 1);  
    cboxLayout->addWidget(frameControlBar,   cboxLayout_Line_1, 10, 1, 1);

    // ... PK 3-4-21 added image-line-control

    cboxLayout->addWidget(fpsLabel,                   cboxLayout_Line_2, 0, 1, 1);
    cboxLayout->addWidget(new QLabel("Range:", this), cboxLayout_Line_2, 1, 1, 1);
    cboxLayout->addWidget(min_box,                    cboxLayout_Line_2, 2, 1, 1);
    cboxLayout->addWidget(rangeSlider,                cboxLayout_Line_2, 3, 1, 5);
    cboxLayout->addWidget(max_box,                    cboxLayout_Line_2, 8, 1, 1);
    cboxLayout->addWidget(precisionBox, cboxLayout_Line_2, 9, 1, 1);
    cboxLayout->addWidget(maskButton, cboxLayout_Line_2, 10, 1, 1);

    cboxLayout->addWidget(ipLabel, cboxLayout_Line_3, 0, 1, 1);
    cboxLayout->addWidget(new QLabel("Save File to:", this), cboxLayout_Line_3, 1, 1, 1);
    cboxLayout->addWidget(saveFileNameEdit, cboxLayout_Line_3, 2, 1, 5);
    cboxLayout->addWidget(browseButton, cboxLayout_Line_3, 7, 1, 1);
    cboxLayout->addWidget(saveFramesButton, cboxLayout_Line_3, 8, 1, 1);
    cboxLayout->addWidget(new QLabel("Num. Frames:", this), cboxLayout_Line_3, 9, 1, 1);
    cboxLayout->addWidget(numFramesEdit, cboxLayout_Line_3, 10, 1, 1);

    cboxLayout->addWidget(portLabel, cboxLayout_Line_4, 0, 1, 1);
    cboxLayout->addWidget(new QLabel("Std. Dev. N:", this), cboxLayout_Line_4, 1, 1, 1);
    cboxLayout->addWidget(stdDevNBox, cboxLayout_Line_4, 2, 1, 1);
    cboxLayout->addWidget(stdDevNSlider, cboxLayout_Line_4, 3, 1, 5);
    cboxLayout->addWidget(new QLabel("Num. Avgs:", this), cboxLayout_Line_4, 9, 1, 1);
    cboxLayout->addWidget(numAvgsEdit, cboxLayout_Line_4, 10, 1, 1);

    this->setLayout(cboxLayout);
    // 3-3-21 image-line-control
    //
    // Increase the height because added another line for image-line-control
    // original - this->setMaximumHeight(150);

    // latest - this->setMaximumHeight(190);
    this->setMaximumHeight(220);  // 3-15-21 frame control status widgets enhancement
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

} // end of ControlsBox::ControlsBox()

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
    if (!saveFileNameEdit->text().isEmpty() && numFramesEdit->value() != 0) {
        save_req_t new_req = {bit_org,
                              findAndReplaceFileName(saveFileNameEdit->text()).toStdString(),
                              static_cast<int64_t>(numFramesEdit->value()),
                              static_cast<int64_t>(numAvgsEdit->value())};
        frame_handler->saveFrames(new_req);
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

//
// PK EMITFPIED-331_V2 Frame Control button enhancement
// 
// add Frame Control buttons: Rewind, Play/Stop, Forward
void ControlsBox::frameControlPrevButtonClicked()
{
    qDebug() << "\nPK Debug - frameControlStopButtonClicked() - PREV button Clicked ...\n";

    if( frameDisplay->IsFrameLineControlEnabled() )
        frameDisplay->resetFrameLineDisplay( true );
    
} // end of ControlsBox::frameControlPrevButtonClicked()

void ControlsBox::frameControlNextButtonClicked()
{
    qDebug() << "\nPK Debug - frameControlNextButtonClicked() - NEXT button Clicked ...\n";

    //
    // Ignore the Next button press event if Frame Control is OFF
    if( frameWorkerParent->isFrameControlOn() == false )
    {
        qDebug() << "\nPK Debug - frameControlNextButtonClicked() - frameControlOn is OFF";
        return;
    }


    if( frameDisplay->IsFrameLineControlEnabled() == false )
    {
        // PK 1-13-21 added ...
        frameWorkerParent->setFrameAcquisitionFrameCount( 1 ); 
        frameWorkerParent->setFrameControlFrameCount( 1 );
        qDebug() << "\nPK Debug - ControlsBox::frameControlNextButtonClicked() - frameControlFrameCount is set to " << frameWorkerParent->getFrameControlFrameCount();
        // PK 1-13-21 added ...
    }
    else
    {
        qDebug() << "\nPK Debug - frameControlNextButtonClicked() - displayNextFrameLine is set ...\n";
        frameDisplay->forwardToNextFrameLine( true );
    }
    
} // end of ControlsBox::frameControlNextButtonClicked()


enum frameControlButton{
    STOP_button = 1,
    PLAY_button = 2
};


void ControlsBox::frameControlStopButtonClicked()
{
    static int currDisplayButton = STOP_button;

    //
    // Note, it always starts with a STOP button as frames are being
    // displayed (played) by LiveView when LiveView starts.
    switch( currDisplayButton )
    {
    case STOP_button:
        qDebug() << "\nPK Debug - frameControlStopButtonClicked() - STOP button Clicked, switch to PLAY button. \n";

        //
        // while frames are being 'played', a 'STOP' button is always displayed.
        // Swith to a PLAY button when the 'STOP' button is clicked.
        frameControl_stopButton->setIcon( style()->standardIcon(QStyle::SP_MediaPlay) );
        currDisplayButton = PLAY_button;

        if( frameWorkerParent->isFrameControlOn() == false )
        {
            //
            // turn On frameControl, and suspend frame display
            frameWorkerParent->setFrameControlStatus( true );
            frameWorkerParent->suspendFrameAcquisition();
            frameControlStatus->setText( QString::fromStdString( "Frame Control:  ON ") );
        }
        break;

    case PLAY_button:
        //
        // while frames playing are being suspended, a 'PLAY' button is shown !!
        // Swith to a STOP button when the 'PLAY' button is clicked.
        qDebug() << "\nPK Debug - frameControlStopButtonClicked() - PLAY button Clicked, switch to STOP button.\n";
        frameControl_stopButton->setIcon( style()->standardIcon(QStyle::SP_MediaStop) );
        currDisplayButton = STOP_button;

        frameWorkerParent->setFrameControlStatus( false );  
        frameWorkerParent->resumeFrameAcquisition();
        frameControlStatus->setText( QString::fromStdString( "Frame Control:  OFF ") );


        if( frameDisplay->IsFrameLineControlEnabled() == true )
        {
            frameDisplay->updateFrameLineControlStatus( false );
            qDebug() << "\nPK Debug ControlsBox::frameControlStopButtonClicked() frame line control:" << frameDisplay->IsFrameLineControlEnabled();
            qDebug() << "\nPK Debug disabled frameLineControl ...";
            //
            // Clear all frame line control labels
            frameLineNo->setText( QString::fromStdString("Line #: ") );
            frameLineCount->setText( QString::fromStdString("Line Count: ") );
            frameCollectionID->setText( QString::fromStdString("Collection ID: ") );
            frameLineControlStatus->setText( QString::fromStdString( "Frame Line Control:  OFF ") );
            frameLineControl->setChecked( false );
        }
            
        break;
    }

} // end of ControlsBox::frameControlStopButtonClicked()

// PK EMITFPIED-331_V2 Frame Control button enhancement


void ControlsBox::frameLineControlChecked( bool enabled )
{
    qDebug() << "\nPK Debug - ControlsBox::frameLineControlChecked() - enabled:" << enabled;
    if( frameWorkerParent->isFrameControlOn() == false )
    {
        //
        // no action taken ...  Frame line control can ONLY be enabled
        // when frame control is ON.
        qDebug() << "\nPK Debug frameLineControlChecked() frame line control enable/disable IGNORED !!! because frame control is DISABLED.";
        return;
    }

    //
    // Signal to start frame line control ... and update 
    //  frame line control status
    if( frameDisplay->IsFrameLineControlEnabled() == false )
    {
        // 3-15-21 frame-line-control enhancement
        frameDisplay->updateFrameLineControlStatus( true ); 

        //
        // testing take it out for now
        // emit startFrameLineControl();
        qDebug() << "\nPK Debug ControlsBox::frameLineControlChecked() frame line control:" << frameDisplay->IsFrameLineControlEnabled();
        frameLineControlStatus->setText( QString::fromStdString( "Frame Line Control:  ON") );

    }
    else
    {
        // 3-15-21 frame-line-control enhancement
        frameDisplay->updateFrameLineControlStatus( false ); 

        qDebug() << "\nPK Debug ControlsBox::frameLineControlChecked() frame line control:" << frameDisplay->IsFrameLineControlEnabled(); 

        frameLineControlStatus->setText( QString::fromStdString( "Frame Line Control:  OFF") );

        //
        // Clear all frame line control labels
        frameLineNo->setText( QString::fromStdString("Line #: ") );
        frameLineCount->setText( QString::fromStdString("Line Count: ") );
        frameCollectionID->setText( QString::fromStdString("Collection ID: ") );
    }

} // end of frameLineControlChecked()


void ControlsBox::updateFrameFilename( const std::string filename )
{
    qDebug() << "PK Debug ControlsBox::updateFrameFilename:" << filename.data();
    std::string labelStr = "Image Filename: " + filename;
    frameFilename->setText( QString::fromStdString(labelStr) );
} // end of updateFrameFilename()



void ControlsBox::updateFrameLineInfo() // 3-10-21 image-line-control
{
    qDebug() << "PK Debug ControlsBox::updateFrameLineInfo signal RECEIVED!!";
    
    frameLineControlData   info;

    // PK 3-15-21 frame-line-control enhancement ... 
    frameLineDisplayInfoMutex.lock();  
    info = frameLineDisplayInfo;
    frameLineDisplayInfoMutex.unlock();
    // ... PK 3-15-21 frame-line-control enhancement

    if( info.line_no < 0 )
    {
        //
        // Negative line_no indicates it's an error, disable frame line control
        frameDisplay->updateFrameLineControlStatus( false ); 

        qDebug() << "\nPK Debug ControlsBox::updateFrameLineInfo() disable frame line control due to error";

        frameLineControlStatus->setText( QString::fromStdString( "Frame Line Control:  OFF") );

        //
        // Clear all frame line control labels
        frameLineNo->setText( QString::fromStdString("Line #: ") );
        frameLineCount->setText( QString::fromStdString("Line Count: ") );
        frameCollectionID->setText( QString::fromStdString("Collection ID: ") );
        frameLineControl->setChecked( false );
        
    }
    else
    {
        qDebug() << "PK Debug ControlsBox::updateFrameLineInfo line#:" << info.line_no;
        std::string line_no_label = "Line #: " + std::to_string(info.line_no);
        frameLineNo->setTextFormat( Qt::PlainText );
        frameLineNo->setText( QString::fromStdString(line_no_label) );

        qDebug() << "PK Debug ControlsBox::updateFrameLineInfo lineCount:" << info.lineCount;
        std::string lineCount_label = "Line Count: " + std::to_string(info.lineCount);
        frameLineCount->setTextFormat( Qt::PlainText );
        frameLineCount->setText( QString::fromStdString(lineCount_label) );

        qDebug() << "PK Debug ControlsBox::updateFrameLineInfo CollectionId:" << info.dataId;
        std::string collectionID_label = "Collection ID: " + std::to_string(info.dataId);
        frameCollectionID->setTextFormat( Qt::PlainText );
        frameCollectionID->setText( QString::fromStdString(collectionID_label) );
    }


} // end of updateFrameLineInfo()

