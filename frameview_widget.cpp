#include "frameview_widget.h"

frameview_widget::frameview_widget(image_t image_type, FrameWorker* fw, QWidget *parent) :
        QWidget(parent),
        image_type(image_type)
{
    this->image_type = image_type;
    frame_handler = fw;

    floor = 0;
    ceiling = UINT16_MAX;

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
    colorMapData = nullptr;

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

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(qcp);
    this->setLayout(layout);

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

double frameview_widget::getCeiling()
{
    /*! \brief Return the value of the ceiling for this widget as a double */
    return ceiling;
}
double frameview_widget::getFloor()
{
    /*! \brief Return the value of the floor for this widget as a double */
    return floor;
}

void frameview_widget::handleNewFrame()
{
    uint16_t* image_data;
    if (frame_handler->running()) {
        image_data = frame_handler->getFrame();
        for (int col = 0; col < frWidth; col++) {
            for (int row = 0; row < frHeight; row++ ) {
                colorMap->data()->setCell(col, row, image_data[row * frWidth + col]); // y-axis NOT reversed
            }
        }
        qcp->replot();
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
void frameview_widget::updateCeiling(int c)
{
    /*! \brief Change the value of the ceiling for this widget to the input parameter and replot the color scale. */
    ceiling = (double)c;
    rescaleRange();
}
void frameview_widget::updateFloor(int f)
{
    /*! \brief Change the value of the floor for this widget to the input parameter and replot the color scale. */
    floor = (double)f;
    rescaleRange();
}
void frameview_widget::rescaleRange()
{
    /*! \brief Set the color scale of the display to the last used values for this widget */
    colorScale->setDataRange(QCPRange(floor,ceiling));
}

