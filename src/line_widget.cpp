#include "line_widget.h"

line_widget::line_widget(FrameWorker *fw, image_t image_t, QWidget *parent) :
    LVTabApplication(fw, parent), image_type(image_t)
{
    qcp->addLayer("Plot Layer");
    qcp->setCurrentLayer("Plot Layer");
    qcp->setAntialiasedElement(QCP::aeAll);

    qcp->plotLayout()->insertRow(0);
    plotTitle = new QCPTextElement(qcp, "No crosshair selected");
    plotTitle->setFont(QFont(font().family(), 20));
    qcp->plotLayout()->addElement(0, 0, plotTitle);

    qcp->addGraph(0, 0);

    switch (image_type) {
    case SPECTRAL_PROFILE:
        xAxisMax = frHeight;
        qcp->xAxis->setLabel("Spectral index");
        p_getLine = &line_widget::getSpectralLine;
        break;
    case SPATIAL_PROFILE:
        xAxisMax = frWidth;
        qcp->xAxis->setLabel("Spatial index");
        p_getLine = &line_widget::getSpatialLine;
        break;
    default:
        xAxisMax = frHeight;
        qcp->xAxis->setLabel("Spectral index");
        p_getLine = &line_widget::getSpectralLine;
    }

    upperRangeBoundX = xAxisMax;

    x = QVector<double>(xAxisMax);
    for (int i = 0; i < xAxisMax; i++) {
        x[i] = double(i);
    }
    y = QVector<double>(xAxisMax);
    qcp->xAxis->setRange(QCPRange(0, xAxisMax));

    qcp->setInteractions(QCP::iRangeZoom | QCP::iSelectItems);

    qcp->yAxis->setLabel("Pixel Magnitude [DN]");
    setCeiling((1<<16) -1);
    setFloor(0);

    qcp->graph(0)->setData(x, y);

    qcp->addLayer("Box Layer", qcp->currentLayer());
    qcp->setCurrentLayer("Box Layer");

    callout = new QCPItemText(qcp);
    callout->setFont(QFont(font().family(), 16));
    callout->setSelectedFont(QFont(font().family(), 16));
    callout->setVisible(false);
    callout->position->setCoords(xAxisMax / 2, getCeiling() - 3000);

    tracer = new QCPItemTracer(qcp);
    tracer->setGraph(qcp->graph());
    tracer->setStyle(QCPItemTracer::tsNone);
    tracer->setInterpolating(true);
    tracer->setVisible(false);

    qcp->addLayer("Trace Layer", qcp->currentLayer(), QCustomPlot::limBelow);
    qcp->setCurrentLayer("Trace Layer");

    arrow = new QCPItemLine(qcp);
    arrow->end->setCoords(-1.0, -1.0);
    arrow->start->setParentAnchor(callout->bottom);
    arrow->end->setParentAnchor(tracer->position);
    arrow->setHead(QCPLineEnding::esSpikeArrow);
    arrow->setSelectable(false);
    arrow->setVisible(false);

    if (USE_DARK_STYLE) {
        qcp->graph(0)->setPen(QPen(Qt::lightGray));
        plotTitle->setTextColor(Qt::white);

        qcp->setBackground(QBrush(QColor("#31363B")));
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

        callout->setColor(Qt::white);
        callout->setPen(QPen(Qt::white));
        callout->setBrush(QBrush(QColor("#31363B")));
        callout->setSelectedBrush(QBrush(QColor("#31363B")));
        callout->setSelectedPen(QPen(Qt::white));
        callout->setSelectedColor(Qt::white);

        arrow->setPen(QPen(Qt::white));
    } else {
        callout->setPen(QPen(Qt::black));
        callout->setBrush(Qt::white);
        callout->setSelectedBrush(Qt::white);
        callout->setSelectedPen(QPen(Qt::black));
        callout->setSelectedColor(Qt::black);
    }

    QVBoxLayout *qvbl = new QVBoxLayout;
    qvbl->addWidget(qcp);
    this->setLayout(qvbl);

    connect(qcp->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(graphScrolledX(QCPRange)));
    connect(qcp->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(lineScrolledY(QCPRange)));
    connect(frame_handler, SIGNAL(crosshairChanged(QPointF)), this, SLOT(updatePlotTitle(QPointF)));
    connect(qcp, SIGNAL(plottableDoubleClick(QCPAbstractPlottable*,int,QMouseEvent*)), \
            this, SLOT(setTracer(QCPAbstractPlottable*,int,QMouseEvent*)));
    connect(qcp, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(moveCallout(QMouseEvent*)));
    connect(&renderTimer, SIGNAL(timeout()), this, SLOT(handleNewFrame()));

    renderTimer.start(FRAME_DISPLAY_PERIOD_MSECS);
}

line_widget::~line_widget()
{
}

QVector<double> line_widget::getSpectralLine(QPointF coord)
{
    QVector<double> graphData(frHeight);
    size_t col = (size_t)coord.y();
    float *image_data = frame_handler->getFrame();
    for (size_t r = 0; r < frHeight; r++) {
        graphData[r] = image_data[r * frWidth + col];
    }
    return graphData;
}

QVector<double> line_widget::getSpatialLine(QPointF coord)
{
    QVector<double> graphData(frWidth);
    size_t row = (size_t)coord.x();
    float *image_data = frame_handler->getFrame();
    for (size_t c = 0; c < frWidth; c++) {
        graphData[c] = image_data[row * frWidth + c];
    }
    return graphData;
}

void line_widget::handleNewFrame()
{
    if (!this->isHidden() && frame_handler->running()) {
        QPointF *center = frame_handler->getCenter();
        if (center->x() > -0.1) {
            y = (this->*p_getLine)(*center);
            qcp->graph(0)->setData(x, y);
            callout->setText(QString(" x: %1 \n y: %2 ").arg((int)tracer->graphKey()).arg((int)y[(int)tracer->graphKey()]));
            qcp->replot();
        }
    }
}

void line_widget::rescaleRange()
{
    qcp->yAxis->setRange(QCPRange(floor, ceiling));
}

void line_widget::lineScrolledY(const QCPRange &newRange)
{
    lowerRangeBoundY = dataMin;
    upperRangeBoundY = dataMax;
    this->graphScrolledY(newRange);
    setFloor(qcp->yAxis->range().lower);
    setCeiling(qcp->yAxis->range().upper);
}

void line_widget::updatePlotTitle(const QPointF &coord)
{
    switch(image_type) {
    case SPECTRAL_PROFILE:
        plotTitle->setText(QString("Spectral Profile centered at x = %1").arg((int)coord.x()));
        break;
    case SPATIAL_PROFILE:
        plotTitle->setText(QString("Spatial Profile centered at y = %1").arg((int)coord.y()));
        break;
    default:
        plotTitle->setText(QString("Spectral Profile centered at x = %1").arg((int)coord.x()));
    }
}

void line_widget::setTracer(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event)
{
    Q_UNUSED(plottable);
    Q_UNUSED(dataIndex);
    double dataX = qcp->xAxis->pixelToCoord(event->pos().x());
    double dataY = qcp->yAxis->pixelToCoord(event->pos().y());
    tracer->setGraphKey(dataX);
    qDebug() << "Set Graph Key to " << dataX;
    callout->setText(QString("x: %1 \n y: %2 ").arg((int)dataX).arg((int)dataY));
    if (callout->position->coords().y() > getCeiling() || callout->position->coords().y() < getFloor()) {
        callout->position->setCoords(callout->position->coords().x(), (getCeiling() - getFloor()) * 0.9 + getFloor());
    }
    callout->setVisible(true);
    callout->setSelected(false);
    tracer->setVisible(true);
    arrow->setVisible(true);

}

void line_widget::moveCallout(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton) {
        callout->position->setPixelPosition(e->pos());
    }
}
