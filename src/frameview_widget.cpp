#include <QMutex>         // 3-8-21 frame-line-control
#include "frameview_widget.h"
#include "lvframe.h"      // 3-8-21 frame-line-control


extern bool frameLineDebugLog;   // PK 3-2-21 image-line-debug

// PK 3-5-21 image-line-control ...
extern bool frameLineControlEnabled;
extern bool displayNextFrameLine;
extern bool frameLineDisplayReset;    

frameDataFile *currentFrameDataFile;
// ... PK 3-5-21 image-line-control

extern frameLineControlData   frameLineDisplayInfo;
extern QMutex frameLineDisplayInfoMutex;

#define FRAME_LINE_TOTAL (32)

enum frameLineSteppingStatus {
    Status_noCurrentFrameDataFile    = 0,
    Status_allFrameLinesDisplayed    = 1,
    Status_resetFrameLineDisplay     = 2,
    Status_frameLineControlDisabled  = 3
};


frameview_widget::frameview_widget(FrameWorker *fw,
                                   image_t image_type,
                                   QSettings *set,
                                   QWidget *parent) :
        LVTabApplication(fw, parent),
        image_type(image_type),
        count(0), count_prev(0),
        fps(0), settings(set)
{
    switch(image_type) {
    case BASE:
        ceiling = UINT16_MAX;
        p_getFrame = &FrameWorker::getFrame;
        break;
    case DSF:
        ceiling = 100.0;
        p_getFrame = &FrameWorker::getDSFrame;
        break;
    case STD_DEV:
        ceiling = 100.0;
        p_getFrame = &FrameWorker::getSDFrame;
        break;
    default:
        ceiling = UINT16_MAX;
        p_getFrame = &FrameWorker::getFrame;
    }


    //
    // EMITFPIED-331
    // PK add frame control feature 11-16-20
    // need to coummnicate with FrameWorker thread
    frameWorkerParent = fw;

    floor = 0.0;

    upperRangeBoundX = frWidth - 1;
    upperRangeBoundY = frHeight - 1;

    QSizePolicy qsp(QSizePolicy::Preferred, QSizePolicy::Preferred);
    qsp.setHeightForWidth(true);
    qcp->setSizePolicy(qsp);
    qcp->heightForWidth(200);
    qcp->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    qcp->axisRect()->setupFullAxesBox(true);
    // Removing axis labels to provide more room for the display
    // qcp->xAxis->setLabel("x");
    // qcp->yAxis->setLabel("y");
    qcp->yAxis->setRangeReversed(true);

    colorMap = new QCPColorMap(qcp->xAxis, qcp->yAxis);
    colorMapData = nullptr;

    colorScale = new QCPColorScale(qcp);
    qcp->plotLayout()->addElement(0, 1, colorScale);
    colorScale->setType(QCPAxis::atRight);
    colorScale->setRangeDrag(false);
    colorScale->setRangeZoom(false);

    colorMap->setColorScale(colorScale);
    colorMap->data()->setValueRange(QCPRange(0, frHeight-1));
    colorMap->data()->setKeyRange(QCPRange(0, frWidth-1));
    colorMap->setDataRange(QCPRange(floor, ceiling));
    colorMap->setGradient(QCPColorGradient(
                              static_cast<QCPColorGradient::GradientPreset>(
                                  settings->value(
                                      QString("gradient"),
                                      QCPColorGradient::gpJet).toInt())));
    colorMap->setInterpolate(false);
    colorMap->setAntialiased(false);

    auto marginGroup = new QCPMarginGroup(qcp);
    qcp->axisRect()->setMarginGroup(QCP::msBottom | QCP::msTop, marginGroup);
    colorScale->setMarginGroup(QCP::msBottom | QCP::msTop, marginGroup);

    setDarkMode();

    qcp->rescaleAxes();
    qcp->axisRect()->setBackgroundScaled(false);

    fpsLabel = new QLabel("Starting...");
    fpsLabel->setFixedWidth(150);

    auto zoomOptions = new QComboBox();
    zoomOptions->addItem("Zoom on Both axes");
    zoomOptions->addItem("Zoom on X axis only");
    zoomOptions->addItem("Zoom on Y axis only");
    connect(zoomOptions, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=](int index) {
        switch (index) {
        case 0: // Zoom on Both Axes
            qcp->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
            qcp->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
            break;
        case 1: // Zoom on X Axis Only
            qcp->axisRect()->setRangeZoom(Qt::Horizontal);
            qcp->axisRect()->setRangeDrag(Qt::Horizontal);
            break;
        case 2: // Zoom on Y Axis Only
            qcp->axisRect()->setRangeZoom(Qt::Vertical);
            qcp->axisRect()->setRangeDrag(Qt::Vertical);
            break;
        default: // default behavior is to zoom on both axes
            qcp->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
            qcp->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
        }
    });

    // Create a crosshair made of two 0-width boxes
    // that allows users to select lines of data to
    // view in detail in the "profile" panes
    crosshairX = new QCPItemRect(qcp);
    crosshairX->setPen(QPen(Qt::white));
    crosshairY = new QCPItemRect(qcp);
    crosshairY->setPen(QPen(Qt::white));

    // To start, chuck these crosshair items off the
    // edge of the screen so they can't be seen
    crosshairX->bottomRight->setCoords(-100, -100);
    crosshairX->topLeft->setCoords(-100, -100);
    crosshairY->bottomRight->setCoords(-100, -100);
    crosshairY->topLeft->setCoords(-100, -100);

    tlBox = new QCPItemRect(qcp);
    trBox = new QCPItemRect(qcp);
    blBox = new QCPItemRect(qcp);
    brBox = new QCPItemRect(qcp);

    tlBox->topLeft->setCoords(-100, -100);
    tlBox->bottomRight->setCoords(-100, -100);
    trBox->bottomRight->setCoords(-100, -100);
    trBox->topLeft->setCoords(-100, -100);

    blBox->bottomRight->setCoords(-100, -100);
    blBox->topLeft->setCoords(-100, -100);
    brBox->bottomRight->setCoords(-100, -100);
    brBox->topLeft->setCoords(-100, -100);

    loBoundX = frame_handler->getCenter()->x();
    hiBoundX = frame_handler->getCenter()->x();

    loBoundY = frame_handler->getCenter()->y();
    hiBoundY = frame_handler->getCenter()->y();

    QPen sideCrossPen(QColor(230, 230, 230));
    tlBox->setPen(sideCrossPen);
    trBox->setPen(sideCrossPen);
    blBox->setPen(sideCrossPen);
    brBox->setPen(sideCrossPen);

    QCheckBox *hideXbox = new QCheckBox("Hide Crosshair", this);
    connect(hideXbox, SIGNAL(toggled(bool)), this, SLOT(hideCrosshair(bool)));
    hideXbox->setStyleSheet("QCheckBox { outline: none }");
    hideXbox->setFixedWidth(150);

    QCheckBox *showTipBox = new QCheckBox("Show Value at Cursor", this);
    connect(showTipBox, SIGNAL(toggled(bool)), this, SLOT(showTooltip(bool)));
    showTipBox->setStyleSheet("QCheckBox { outline: none }");
    showTipBox->setFixedWidth(200);

    auto qvbl = new QVBoxLayout(this);
    auto bottomControls = new QHBoxLayout;
    bottomControls->addWidget(fpsLabel);
    bottomControls->addWidget(hideXbox);
    bottomControls->addWidget(showTipBox);

    /* In the dark subtraction mode, add an additional checkbox
     * at the bottom of the pane that allows the user to toggle
     * whether to display the dark subtracted data or the SNR
     * data. The SNR calculation is performed in the
     * FrameWorker::captureSDFrames loop function.
     */
    if (image_type == DSF) { //Dark Sub Widget Only
        QCheckBox *plotModeCheckbox =
                new QCheckBox("Plot Signal-to-Noise Ratio", this);
        connect(plotModeCheckbox, &QCheckBox::toggled,
                this, &frameview_widget::setPlotMode);
        plotModeCheckbox->setStyleSheet("QCheckBox { outline: none }");
        plotModeCheckbox->setFixedWidth(150);
        bottomControls->addWidget(plotModeCheckbox);
    }

    bottomControls->addWidget(zoomOptions);

    qvbl->addWidget(qcp, 10);
    qvbl->addLayout(bottomControls, 1);

    this->setLayout(qvbl);

    connect(&renderTimer, &QTimer::timeout, this, &frameview_widget::handleNewFrame);
    connect(&fpsclock, &QTimer::timeout, this, &frameview_widget::reportFPS);
    connect(qcp->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(graphScrolledY(QCPRange)));
    connect(qcp->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(graphScrolledX(QCPRange)));

    connect(qcp, &QCustomPlot::mousePress, this, &frameview_widget::mouse_down);
    connect(qcp, &QCustomPlot::mouseMove, this, &frameview_widget::mouse_move);
    connect(qcp, &QCustomPlot::mouseRelease, this, &frameview_widget::mouse_up);
    if (image_type == BASE) {
        connect(qcp, &QCustomPlot::plottableDoubleClick, this, &frameview_widget::drawCrosshair);
    }
    colorMapData = new QCPColorMapData(frWidth, frHeight, QCPRange(0, frWidth-1), QCPRange(0, frHeight-1));
    colorMap->setData(colorMapData);

    connect(frame_handler->Camera, &CameraModel::timeout, this, [=]() {
        for (int col = 0; col < frWidth; col++) {
            for (int row = 0; row < frHeight; row++ ) {
                colorMap->data()->setCell(col, row, 0); // y-axis NOT reversed
            }
        }
        qcp->replot();
        // renderTimer.stop();
    });

    if (frame_handler->running()) {
        renderTimer.start(FRAME_DISPLAY_PERIOD_MSECS);
        fpsclock.start(1000); // 1 sec
    }

} // end of frameview_widget::frameview_widget()



//
// PK 1-12-21
//
// frameview_widget::handleNewFrame() is a slot, signal handler.  It process
// signal 'timeout' 
//
void frameview_widget::handleNewFrame() 
{
    //
    // EMITFPIED-331
    // This is where to insert a check for frame display suspension status
    static bool logged = false;

    // 3-10-21 frame-line-contol ...
    //
    // Issue - 
    // handleNewFrame() test code 
    static int frameLineIndex = 0;

    if( frameLineDisplayReset )
    {
        frameLineIndex = 0;
        frameLineDisplayReset = false;
    }

    if( !frameLineControlEnabled ) 
    {
        // Reset line index 
        if( frameLineIndex > 0 ) frameLineIndex = 0;
    }
    else
    {
        //
        // Always display the 1st line when frame line control is ENABLED.
        if( frameLineIndex == 0 )
        {
            frameLineData sLine = getFrameLine( frameLineIndex, *currentFrameDataFile );
            frameLineDisplayInfoMutex.lock();
            {
                frameLineDisplayInfo.line_no = frameLineIndex+1;
                frameLineDisplayInfo.lineCount = sLine.lineCount;
                frameLineDisplayInfo.dataId = sLine.dataId;
            }
            frameLineDisplayInfoMutex.unlock();
            
            displaySingleFrameLine( frameLineIndex, currentFrameDataFile->frameSize, sLine );
            emit updateFrameLineInfo();
            frameLineIndex++;
        }
    }

    if( displayNextFrameLine )
    {
        if( frameLineIndex < FRAME_LINE_TOTAL )
        {
            frameLineData sLine = getFrameLine( frameLineIndex, *currentFrameDataFile );
            frameLineDisplayInfoMutex.lock();
            {
                frameLineDisplayInfo.line_no = frameLineIndex+1;
                frameLineDisplayInfo.lineCount = sLine.lineCount;
                frameLineDisplayInfo.dataId = sLine.dataId;
            }
            frameLineDisplayInfoMutex.unlock();
        

            displaySingleFrameLine( frameLineIndex, currentFrameDataFile->frameSize, sLine );
            emit updateFrameLineInfo();
            frameLineIndex++;
        }

        if( displayNextFrameLine )
            displayNextFrameLine = false;
    }
    // ... 3-10-21 frame-line-contol 

    if( frameWorkerParent->isFrameControlOn() == true &&
        frameWorkerParent->getFrameControlFrameCount() == 0 )
    {
        //
        // if frameControlIsOn, stops grabbing new frames for display.
        if( !logged )
        {
            qDebug() << "PK Debug frameControlIsOn is true, frameview_widget::handleNewFrame() returns and do NOTHING";
            logged = true;
        }

        // sleep for 1 msec
        QThread::usleep( 1000 );
        return;
    }

    //
    // PK 1-13-21 added ... Forward button support
    //
    if( frameWorkerParent->getFrameControlFrameCount() > 0 )
    {
        // gives the thread a chance to fill the lvframe_buf
        // sleep for 500 msec
        qDebug() << "PK Debug frameview_widget::handleNewFrame() - frameControlIsOn with NEXT button clicked, sleeps for 500 ms. \n";
        QThread::usleep( 500000 );
    }

    frameDataFile *LVData;
    std::vector <frameLineData> frameLines;
    //
    // 2-25-21 image-line-control
    //
    // The following line caused image frame missing and not displayed
    // by LiveView.  This is due to timing issue among the threads.
    //
    //  if (!this->isHidden() && frame_handler->Camera->isRunning())
    // 
    // Thus, frame_handler->Camera->isRunning() is replaced with
    // (LVData = frameWorkerParent->getFrameDataFile()) != NULL to ensure
    // all image files (records) in the FrameDataFileList is processed.
    if( !this->isHidden() &&
        (LVData = frameWorkerParent->getFrameDataFile()) != NULL )
    {
        timeout_display = true;

        //
        // image-line-control new feature
        // 
        // instead of calling frame_handler->*p_getFrame()
        // replace it with frameWorkerParent->getFrameDataFile() to
        // obtain a frameDataFile record.
        //
        if( LVData != NULL )
        {
            currentFrameDataFile = LVData;

            if( !frameLineControlEnabled )
            {
                //
                // display current frame filename
                emit updateFrameFilename( LVData->filename );

                //
                // Display the frame lines ...
                displayFrameLines( LVData );
            }
        }
        else
        {
            qDebug() << "PK Debug frameview_widget::handleNewFrame() LVData is NULL ... ";
        }

    }
    else
    {
        if (timeout_display) {
            timeout_display = false;
        }
    }

    // PK 1-13-21 added ... Forward button support
    //
    // reset frameControlFrameCount to 0 after displayed ONE frame 
    if( frameWorkerParent->getFrameControlFrameCount() != 0 )
    {
        qDebug() << "frameview_widget::handleNewFrame() - frameControlIsOn with NEXT button clicked, reset FrameCount to 0\n";
        frameWorkerParent->setFrameControlFrameCount( 0 );
    }


} // end of frameview_widget::handleNewFrame() 

void frameview_widget::reportFPS()
{
    fps = count - count_prev;
    count_prev = count;
    fps_string = QString::number(fps, 'f', 1);
    fpsLabel->setText(QString("Display: %1 fps").arg(fps_string));
}

void frameview_widget::rescaleRange()
{
    colorScale->setDataRange(QCPRange(floor, ceiling));
}

void frameview_widget::drawCrosshair(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event)
{
    Q_UNUSED(plottable);
    Q_UNUSED(dataIndex);
    if (event->button() != Qt::RightButton) {
        double dataX = qcp->xAxis->pixelToCoord(event->pos().x());
        double dataY = qcp->yAxis->pixelToCoord(event->pos().y());
        crosshairX->bottomRight->setCoords(dataX, 0);
        crosshairX->topLeft->setCoords(dataX, frHeight);
        crosshairY->bottomRight->setCoords(0, dataY);
        crosshairY->topLeft->setCoords(frWidth, dataY);
        loBoundX += qcp->xAxis->pixelToCoord(event->pos().x()) - frame_handler->getCenter()->x();
        hiBoundX += qcp->xAxis->pixelToCoord(event->pos().x()) - frame_handler->getCenter()->x();

        loBoundY += qcp->yAxis->pixelToCoord(event->pos().y()) - frame_handler->getCenter()->y();
        hiBoundY += qcp->yAxis->pixelToCoord(event->pos().y()) - frame_handler->getCenter()->y();

        frame_handler->setCenter(dataX, dataY);
        if (boxes_enabled) {
            tlBox->topLeft->setCoords(0, 0);
            tlBox->bottomRight->setCoords(loBoundX, loBoundY);
            trBox->topLeft->setCoords(hiBoundX, 0);
            trBox->bottomRight->setCoords(frWidth, loBoundY);

            blBox->topLeft->setCoords(0, hiBoundY);
            blBox->bottomRight->setCoords(loBoundX, frHeight);
            brBox->topLeft->setCoords(hiBoundX, hiBoundY);
            brBox->bottomRight->setCoords(frWidth, frHeight);
        }
    }
}

void frameview_widget::hideCrosshair(bool hide)
{
    crosshairX->setVisible(!hide);
    crosshairY->setVisible(!hide);
}

void frameview_widget::showTooltip(bool show)
{
    show_tooltip = show;
}

void frameview_widget::setPlotMode(bool checked)
{
    p_getFrame = checked ? &FrameWorker::getDSFrame
                         : &FrameWorker::getSNRFrame;
}

QCPColorMap* frameview_widget::getColorMap()
{
    return this->colorMap;
}

void frameview_widget::setDarkMode()
{
    if (settings->value(QString("dark"), USE_DARK_STYLE).toBool()) {
        qcp->setBackground(QBrush(QColor(0x31363B)));
        qcp->xAxis->setTickLabelColor(Qt::white);
        qcp->xAxis->setBasePen(QPen(Qt::white));
        qcp->xAxis->setLabelColor(Qt::white);
        qcp->xAxis->setTickPen(QPen(Qt::white));
        qcp->xAxis->setSubTickPen(QPen(Qt::white));
        qcp->yAxis->setTickLabelColor(Qt::white);
        qcp->yAxis->setBasePen(QPen(Qt::white));
        qcp->yAxis->setLabelColor(Qt::white);
        qcp->yAxis->setTickPen(QPen(Qt::white));
        qcp->yAxis->setSubTickPen(QPen(Qt::white));
        qcp->xAxis2->setTickLabelColor(Qt::white);
        qcp->xAxis2->setBasePen(QPen(Qt::white));
        qcp->xAxis2->setTickPen(QPen(Qt::white));
        qcp->xAxis2->setSubTickPen(QPen(Qt::white));
        qcp->yAxis2->setTickLabelColor(Qt::white);
        qcp->yAxis2->setBasePen(QPen(Qt::white));
        qcp->yAxis2->setTickPen(QPen(Qt::white));
        qcp->yAxis2->setSubTickPen(QPen(Qt::white));
        colorScale->axis()->setTickLabelColor(Qt::white);
        colorScale->axis()->setBasePen(QPen(Qt::white));
        colorScale->axis()->setLabelColor(Qt::white);
        colorScale->axis()->setTickPen(QPen(Qt::white));
        colorScale->axis()->setSubTickPen(QPen(Qt::white));
    }
}


void frameview_widget::mouse_down(QMouseEvent *event) {
    if(event->button()== Qt::RightButton) {

        bool just_enabled = false;
        if(crosshairX->visible() && !boxes_enabled) {
            boxes_enabled = true;
            just_enabled = true;
        }

        if(boxes_enabled) {
            if((abs(event->pos().x() - qcp->xAxis->coordToPixel(frame_handler->getCenter()->x())) < 50 && just_enabled) ||
               (abs(event->pos().x() - qcp->xAxis->coordToPixel(tlBox->bottomRight->coords().x())) < 50 && event->pos().y() < qcp->yAxis->coordToPixel(tlBox->bottomRight->coords().y()) + 50) ||
               (abs(event->pos().x() - qcp->xAxis->coordToPixel(blBox->bottomRight->coords().x())) < 50 && event->pos().y() > qcp->yAxis->coordToPixel(blBox->topLeft->coords().y()) - 50) ||
               (abs(event->pos().x() - qcp->xAxis->coordToPixel(trBox->topLeft->coords().x())) < 50 && event->pos().y() < qcp->yAxis->coordToPixel(trBox->bottomRight->coords().y()) + 50) ||
               (abs(event->pos().x() - qcp->xAxis->coordToPixel(brBox->topLeft->coords().x())) < 50 && event->pos().y() > qcp->yAxis->coordToPixel(brBox->topLeft->coords().y()) - 50)) {
                dragging_horizontal_box = true;
            }
            if((abs(event->pos().y() - qcp->yAxis->coordToPixel(frame_handler->getCenter()->y())) < 50 && just_enabled) ||
               (abs(event->pos().y() - qcp->yAxis->coordToPixel(tlBox->bottomRight->coords().y())) < 50 && event->pos().x() < qcp->xAxis->coordToPixel(tlBox->bottomRight->coords().x()) + 50) ||
               (abs(event->pos().y() - qcp->yAxis->coordToPixel(blBox->bottomRight->coords().y())) < 50 && event->pos().x() > qcp->xAxis->coordToPixel(blBox->bottomRight->coords().x()) - 50) ||
               (abs(event->pos().y() - qcp->yAxis->coordToPixel(trBox->bottomRight->coords().y())) < 50 && event->pos().x() < qcp->xAxis->coordToPixel(trBox->bottomRight->coords().x()) + 50) ||
               (abs(event->pos().y() - qcp->yAxis->coordToPixel(brBox->bottomRight->coords().y())) < 50 && event->pos().x() > qcp->xAxis->coordToPixel(brBox->bottomRight->coords().x()) - 50)) {
                dragging_vertical_box = true;
            }
            if((abs(event->pos().y() - qcp->yAxis->coordToPixel(frame_handler->getCenter()->y())) < 50 && just_enabled) ||
               abs(event->pos().y() - qcp->yAxis->coordToPixel(tlBox->bottomRight->coords().y())) < 50 ||
               abs(event->pos().y() - qcp->yAxis->coordToPixel(trBox->bottomRight->coords().y())) < 50 ||
               abs(event->pos().y() - qcp->yAxis->coordToPixel(blBox->topLeft->coords().y())) < 50 ||
               abs(event->pos().y() - qcp->yAxis->coordToPixel(brBox->topLeft->coords().y())) < 50) {
                dragging_vertical_box = true;
            }
        }
	  
    }  // bottom of Qt::RightButton

} // end of frameview_widget::mouse_down(QMouseEvent *event)

void frameview_widget::mouse_move(QMouseEvent *event) {
    if (show_tooltip) {
        double x, y, val;
        colorMap->pixelsToCoords(event->pos().x(), event->pos().y(), x, y);
        val = colorMap->data()->data(x, y);

        if (x > 0 && y > 0 && val > 0) {
            QToolTip::showText(event->globalPos(),
                               "X: " + QString::number(x) + "\n"
                               + "Y: " + QString::number(y) + "\n"
                               + "Value: " + QString::number(val),
                               this, rect());
        }
    }

    if(boxes_enabled) {
        if(dragging_horizontal_box) {
            double limitx = qcp->xAxis->pixelToCoord(event->pos().x());
            if(limitx < frame_handler->getCenter()->x()) {
                loBoundX = limitx; //2a - b = a + (a - b)
                hiBoundX = 2 * frame_handler->getCenter()->x() - limitx; //2a - b = a + (a - b)
            } else {
                hiBoundX = limitx; //2a - b = a - (b - a)
                loBoundX = 2 * frame_handler->getCenter()->x() - limitx; //2a - b = a + (a - b)
            }
        }

        if(dragging_vertical_box) {
            double limity = qcp->yAxis->pixelToCoord(event->pos().y());
            if(limity < frame_handler->getCenter()->y()) {
                loBoundY = limity; //2a - b = a + (a - b)
                hiBoundY = 2 * frame_handler->getCenter()->y() - limity; //2a - b = a + (a - b)
            } else {
                hiBoundY = limity; //2a - b = a + (a - b)
                loBoundY = 2 * frame_handler->getCenter()->y() - limity; //2a - b = a + (a - b)
            }
        }

        tlBox->topLeft->setCoords(0, 0);
        tlBox->bottomRight->setCoords(loBoundX, loBoundY);
        trBox->topLeft->setCoords(hiBoundX, 0);
        trBox->bottomRight->setCoords(frWidth, loBoundY);

        blBox->topLeft->setCoords(0, hiBoundY);
        blBox->bottomRight->setCoords(loBoundX, frHeight);
        brBox->topLeft->setCoords(hiBoundX, hiBoundY);
        brBox->bottomRight->setCoords(frWidth, frHeight);
    }
}

void frameview_widget::mouse_up(QMouseEvent *event) {
   mouse_move(event);
   dragging_horizontal_box = false;
   dragging_vertical_box = false;
   double brx = hiBoundX;
   double bry = hiBoundY;
   double tlx = loBoundX;
   double tly = loBoundY;
   // if the side cross hairs have not been moved in a particular
   // dimension, treat it as though the whole range of values is
   // selected rather than just a single line.
   if (int(hiBoundX) == int(loBoundX)) {
        brx = frWidth;
        tlx = 0;
   }
   if (int(hiBoundY) == int(loBoundY)) {
        bry = frHeight;
        tly = 0;
   }
   frame_handler->bottomRight = QPointF(brx, bry);
   frame_handler->topLeft = QPointF(tlx, tly);
}

/****************************************************************
 *
 * Function: displayFrameLines()
 *
 *           This function displays all 32 image frame lines 
 *           onto LiveView window.
 *
 * Returns:  nothing.
 * 
 ****************************************************************/
void frameview_widget::displayFrameLines( frameDataFile *LVData )
{
    //
    // It's no longer a SINGLE write to the lvframe_buffer because 
    // now we have a full collection of 32 frame lines of an image
    // frame to process !!
    std::vector <frameLineData> frameLines = LVData->lineData;
    size_t frameSizeInPixel = (LVData->frameSize)/2;
 
    int lineNo = 0;
    for( frameLineData line : frameLines )
    {
        std::vector<float> image_data(size_t(frameSizeInPixel), 0);
        for( unsigned int i = 0; i < frameSizeInPixel; i++ )
        {
            image_data[i] = float (line.data[i]);
        }
                
        for (int col = 0; col < frWidth; col++) {
            for (int row = 0; row < frHeight; row++ ) {
                colorMap->data()->setCell(col, row,
                                          double(image_data[size_t(row * frWidth + col)])); // y-axis NOT reversed
            }
        }
        qcp->replot();
        if( frameLineDebugLog )  // PK 3-2-21 image-line-debug
            qDebug() << "PK Debug frameview_widget::handleNewFrame() qcp->replot() done, count: " << count;
        count++;
        lineNo++;

    } // bottom of the frame line loop

} // end of frameview_widget::displayFrameLines()


frameLineData frameview_widget::getFrameLine( int i, frameDataFile fData )
{
    std::vector <frameLineData> frameLines = fData.lineData;
    frameLineData line = frameLines[i];
    return line;
} // end of frameview_widget::getFrameLine()

void frameview_widget::displaySingleFrameLine( int lineNo, size_t frameSizeInPixel, frameLineData line )
{
    qDebug() << "PK Debug frameview_widget::displaySingleFrameLine() starts ...";
    qDebug() << "PK Debug lineNo:" << lineNo;
    qDebug() << "PK Debug lineCount:" << line.lineCount;
    qDebug() << "PK Debug collectionId:" << line.dataId;
    qDebug() << "PK Debug frameview_widget::displaySingleFrameLine() ends ...";

    std::vector<float> image_data(size_t(frameSizeInPixel), 0);
    for( unsigned int i = 0; i < frameSizeInPixel; i++ )
    {
        image_data[i] = float (line.data[i]);
    }
                
    for (int col = 0; col < frWidth; col++) {
        for (int row = 0; row < frHeight; row++ ) {
            colorMap->data()->setCell(col, row,
                                      double(image_data[size_t(row * frWidth + col)])); // y-axis NOT reversed
        }
    }
    qcp->replot();
    // if( frameLineDebugLog )  // PK 3-2-21 image-line-debug
    qDebug() << "PK Debug - displaySingleFrameLine() qcp->replot() done ";

} // end of frameview_widget::displaySingleFrameLine()

