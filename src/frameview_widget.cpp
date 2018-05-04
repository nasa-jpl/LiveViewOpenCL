#include "frameview_widget.h"

frameview_widget::frameview_widget(image_t image_type, FrameWorker* fw, QWidget *parent) :
        LVTabApplication(parent),
        image_type(image_type)
{
    this->image_type = image_type;
    frame_handler = fw;

    switch(image_type) {
    case BASE:
        ceiling = float(UINT16_MAX);
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
        p_getFrame = &FrameWorker::getDSFrame;
    }

    floor = 0.0;

    frHeight = frame_handler->getFrameHeight();
    frWidth = frame_handler->getFrameWidth();

    qcp = new QCustomPlot(this);
    qcp->setNotAntialiasedElement(QCP::aeAll);
    QSizePolicy qsp(QSizePolicy::Preferred, QSizePolicy::Preferred);
    qsp.setHeightForWidth(true);
    qcp->setSizePolicy(qsp);
    qcp->heightForWidth(200);
    qcp->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    qcp->axisRect()->setupFullAxesBox(true);
    qcp->xAxis->setLabel("x");
    qcp->yAxis->setLabel("y");
    qcp->yAxis->setRangeReversed(true);

    colorMap = new QCPColorMap(qcp->xAxis, qcp->yAxis);
    colorMapData = NULL;

    // qcp->addPlottable(colorMap);

    colorScale = new QCPColorScale(qcp);
    qcp->plotLayout()->addElement(0, 1, colorScale);
    colorScale->setType(QCPAxis::atRight);
    colorMap->setColorScale(colorScale);
    colorMap->data()->setValueRange(QCPRange(0, frHeight-1));
    colorMap->data()->setKeyRange(QCPRange(0, frWidth-1));
    colorMap->setDataRange(QCPRange(floor, ceiling));
    colorMap->setGradient(QCPColorGradient::gpJet);
    colorMap->setInterpolate(false);
    colorMap->setAntialiased(false);

    QCPMarginGroup* marginGroup = new QCPMarginGroup(qcp);
    qcp->axisRect()->setMarginGroup(QCP::msBottom | QCP::msTop, marginGroup);
    colorScale->setMarginGroup(QCP::msBottom | QCP::msTop, marginGroup);

    qcp->rescaleAxes();
    qcp->axisRect()->setBackgroundScaled(false);

    fpsLabel = new QLabel("Starting...");
    fpsLabel->setFixedWidth(150);

    QGroupBox* zoomButtons = new QGroupBox("Zoom Controls");
    QRadioButton* zoomXButton = new QRadioButton("Zoom on &X axis only");
    connect(zoomXButton, SIGNAL(released()), this, SLOT(setScrollX()));
    QRadioButton* zoomYButton = new QRadioButton("Zoom on &Y axis only");
    connect(zoomYButton, SIGNAL(released()), this, SLOT(setScrollY()));
    QRadioButton* zoomBothButton = new QRadioButton("Zoom on &Both axes");
    connect(zoomBothButton, SIGNAL(released()), this, SLOT(setScrollBoth()));
    zoomBothButton->setChecked(true);

    QHBoxLayout* boxLayout = new QHBoxLayout;
    boxLayout->addWidget(zoomBothButton);
    boxLayout->addWidget(zoomXButton);
    boxLayout->addWidget(zoomYButton);
    // boxLayout->addStretch(1);
    zoomButtons->setLayout(boxLayout);

    QVBoxLayout* vbox = new QVBoxLayout;
    QHBoxLayout* bottomControls = new QHBoxLayout;
    bottomControls->addWidget(fpsLabel);
    bottomControls->addWidget(zoomButtons);
    vbox->addWidget(qcp, 10);
    vbox->addLayout(bottomControls, 1);

    this->setLayout(vbox);

    fps = 0;
    fpsclock.start();

    connect(&rendertimer, SIGNAL(timeout()), this, SLOT(handleNewFrame()));
    connect(qcp->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(colorMapScrolledY(QCPRange)));
    connect(qcp->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(colorMapScrolledX(QCPRange)));

    colorMapData = new QCPColorMapData(frWidth, frHeight, QCPRange(0, frWidth-1), QCPRange(0, frHeight-1));
    colorMap->setData(colorMapData);
    if (frame_handler->running()) {
        rendertimer.start(FRAME_DISPLAY_PERIOD_MSECS);
    }
}

frameview_widget::~frameview_widget()
{
    delete qcp;
}

void frameview_widget::handleNewFrame()
{
    if (!this->isHidden() && (frame_handler->running())) {

        float* image_data = (frame_handler->*p_getFrame)();
        for (unsigned int col = 0; col < frWidth; col++) {
            for (unsigned int row = 0; row < frHeight; row++ ) {
                colorMap->data()->setCell(col, row, image_data[row * frWidth + col]); // y-axis NOT reversed
            }
        }
        qcp->replot();
        count++;
    }
    if (count % 50 == 0 && count != 0) {
        fps = 50.0 / fpsclock.restart() * 1000.0;
        fps_string = QString::number(fps, 'f', 1);
        fpsLabel->setText(QString("Display: %1 fps").arg(fps_string));
    }
}

void frameview_widget::colorMapScrolledY(const QCPRange &newRange)
{
    /*! \brief Controls the behavior of zooming the plot.
     * \param newRange Mouse wheel scrolled range.
     * Color Maps must not allow the user to zoom past the dimensions of the frame.
     */
    QCPRange boundedRange = newRange;
    double lowerRangeBound = 0;
    double upperRangeBound = frHeight-1;
    if (boundedRange.size() > upperRangeBound - lowerRangeBound) {
        boundedRange = QCPRange(lowerRangeBound, upperRangeBound);
    } else {
        double oldSize = boundedRange.size();
        if (boundedRange.lower < lowerRangeBound) {
            boundedRange.lower = lowerRangeBound;
            boundedRange.upper = lowerRangeBound+oldSize;
        } if (boundedRange.upper > upperRangeBound) {
            boundedRange.lower = upperRangeBound - oldSize;
            boundedRange.upper = upperRangeBound;
        }
    }
    qcp->yAxis->setRange(boundedRange);
}
void frameview_widget::colorMapScrolledX(const QCPRange &newRange)
{
    /*! \brief Controls the behavior of zooming the plot.
     * \param newRange Mouse wheel scrolled range.
     * Color Maps must not allow the user to zoom past the dimensions of the frame.
     */
    QCPRange boundedRange = newRange;
    double lowerRangeBound = 0;
    double upperRangeBound = frWidth-1;
    if (boundedRange.size() > upperRangeBound - lowerRangeBound) {
        boundedRange = QCPRange(lowerRangeBound, upperRangeBound);
    } else {
        double oldSize = boundedRange.size();
        if (boundedRange.lower < lowerRangeBound) {
            boundedRange.lower = lowerRangeBound;
            boundedRange.upper = lowerRangeBound + oldSize;
        }
        if (boundedRange.upper > upperRangeBound) {
            boundedRange.lower = upperRangeBound - oldSize;
            boundedRange.upper = upperRangeBound;
        }
    }
    qcp->xAxis->setRange(boundedRange);
}

void frameview_widget::setScrollBoth()
{
    qcp->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    qcp->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
}

void frameview_widget::setScrollX()
{
    qcp->axisRect()->setRangeZoom(Qt::Horizontal);
    qcp->axisRect()->setRangeDrag(Qt::Horizontal);
}

void frameview_widget::setScrollY()
{
    qcp->axisRect()->setRangeZoom(Qt::Vertical);
    qcp->axisRect()->setRangeDrag(Qt::Vertical);
}

void frameview_widget::rescaleRange()
{
    /*! \brief Set the color scale of the display to the last used values for this widget */
    colorScale->setDataRange(QCPRange(floor, ceiling));
}

