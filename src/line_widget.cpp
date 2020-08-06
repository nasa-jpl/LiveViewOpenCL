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

    qcp->addGraph(nullptr, nullptr);

    switch (image_type) {
    case SPATIAL_PROFILE:
        xAxisMax = static_cast<int>(frWidth);
        qcp->xAxis->setLabel("Spatial index");
        p_getLine = &line_widget::getSpatialLine;
        break;
    case SPECTRAL_MEAN:
        xAxisMax = static_cast<int>(frHeight);
        qcp->xAxis->setLabel("Spectral index");
        p_getLine = &line_widget::getSpectralMean;
        plotTitle->setText(QString("Spectral Mean of Single Frame"));
        break;
    case SPATIAL_MEAN:
        xAxisMax = static_cast<int>(frWidth);
        qcp->xAxis->setLabel("Spatial index");
        p_getLine = &line_widget::getSpatialMean;
        plotTitle->setText(QString("Spatial Mean of Single Frame"));
        break;
    case SPECTRAL_PROFILE:
        xAxisMax = static_cast<int>(frHeight);
        qcp->xAxis->setLabel("Spectral index");
        p_getLine = &line_widget::getSpectralLine;
        break;
    default:
        xAxisMax = static_cast<int>(frHeight);
        qcp->xAxis->setLabel("Spectral index");
        p_getLine = &line_widget::getSpectralLine;
    }

    p_getFrame = &FrameWorker::getFrame;

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

    setDarkMode(fw->settings->value(QString("dark"), USE_DARK_STYLE).toBool());

    hideTracer = new QCheckBox("Hide Callout Box", this);
    connect(hideTracer, SIGNAL(toggled(bool)), this, SLOT(hideCallout(bool)));
    hideTracer->setStyleSheet("QCheckBox { outline: none }");

    plotModeBox = new QComboBox();
    plotModeBox->addItem("Raw Data");
    plotModeBox->addItem("Dark Subtracted Data");
    plotModeBox->addItem("Signal-to-Noise Ratio Data");
    connect(plotModeBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setPlotMode(int)));

    auto bottomButtons = new QHBoxLayout;
    bottomButtons->addWidget(hideTracer);
    bottomButtons->addWidget(plotModeBox);

    auto qvbl = new QVBoxLayout(this);
    qvbl->addWidget(qcp);
    qvbl->addLayout(bottomButtons);
    this->setLayout(qvbl);

    connect(qcp->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(graphScrolledX(QCPRange)));
    connect(qcp->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(lineScrolledY(QCPRange)));
    connect(frame_handler, &FrameWorker::crosshairChanged, this, &line_widget::updatePlotTitle);
    connect(qcp, &QCustomPlot::plottableDoubleClick, this, &line_widget::setTracer);
    connect(qcp, &QCustomPlot::mouseMove, this, &line_widget::moveCallout);
    connect(&renderTimer, &QTimer::timeout, this, &line_widget::handleNewFrame);

    connect(frame_handler->Camera, &CameraModel::timeout, this, [=]() {
        QVector<double> zero_data(x.length(), 0);
        qcp->graph(0)->setData(x, zero_data);
        qcp->replot();
        // renderTimer.stop();
    });

    if (frame_handler->running()) {
        renderTimer.start(FRAME_DISPLAY_PERIOD_MSECS);
    }
}

QVector<double> line_widget::getSpectralLine(QPointF coord)
{
    QVector<double> graphData(static_cast<int>(frHeight));
    auto col = int(coord.y());
    std::vector<float> image_data = (frame_handler->*p_getFrame)();
    for (int r = 0; r < frHeight; r++) {
        graphData[r] = static_cast<double>(image_data[size_t(r * frWidth + col)]);
    }
    return graphData;
}

QVector<double> line_widget::getSpatialLine(QPointF coord)
{
    QVector<double> graphData(static_cast<int>(frWidth));
    int row = int(coord.x());
    std::vector<float> image_data = (frame_handler->*p_getFrame)();
    for (int c = 0; c < frWidth; c++) {
        graphData[c] = static_cast<double>(image_data[size_t(row * frWidth + c)]);
    }
    return graphData;
}

QVector<double> line_widget::getSpectralMean(QPointF coord)
{
    Q_UNUSED(coord);
    QVector<double> graphData(static_cast<int>(frHeight));
    float *mean_data = frame_handler->getSpectralMean();
    for (int r = 0; r < frHeight; r++) {
        graphData[r] = static_cast<double>(mean_data[r]);
    }
    return graphData;
}

QVector<double> line_widget::getSpatialMean(QPointF coord)
{
    Q_UNUSED(coord);
    QVector<double> graphData(static_cast<int>(frWidth));
    float *mean_data = frame_handler->getSpatialMean();
    for (int c = 0; c < frWidth; c++) {
        graphData[c] = static_cast<double>(mean_data[c]);
    }
    return graphData;
}

void line_widget::handleNewFrame()
{
    if (!this->isHidden() && frame_handler->running()) {
        QPointF *center = frame_handler->getCenter();
        if (image_type == SPECTRAL_MEAN || image_type == SPATIAL_MEAN || center->x() > -0.1) {
            y = (this->*p_getLine)(*center);
            qcp->graph(0)->setData(x, y);
            // replotting is slow when the data set is chaotic... TODO: develop an optimization here
            qcp->replot();
            if (!hideTracer->isChecked()) {
                callout->setText(QString(" x: %1 \n y: %2 ").arg(static_cast<int>(tracer->graphKey()))
                             .arg(y[static_cast<int>(tracer->graphKey())]));
            }
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
        plotTitle->setText(QString("Spectral Profile centered at x = %1").arg(static_cast<int>(coord.x())));
        break;
    case SPATIAL_PROFILE:
        plotTitle->setText(QString("Spatial Profile centered at y = %1").arg(static_cast<int>(coord.y())));
        break;
    default:
        break;
    }
}

void line_widget::setTracer(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event)
{
    Q_UNUSED(plottable);
    Q_UNUSED(dataIndex);
    double dataX = qcp->xAxis->pixelToCoord(event->pos().x());
    double dataY = qcp->yAxis->pixelToCoord(event->pos().y());
    tracer->setGraphKey(dataX);
    callout->setText(QString("x: %1 \n y: %2 ").arg(static_cast<int>(dataX)).arg(static_cast<int>(dataY)));
    if (callout->position->coords().y() > getCeiling() || callout->position->coords().y() < getFloor()) {
        callout->position->setCoords(callout->position->coords().x(), (getCeiling() - getFloor()) * 0.9 + getFloor());
    }
    if (!hideTracer->isChecked()) {
        callout->setVisible(true);
        callout->setSelected(false);
        tracer->setVisible(true);
        arrow->setVisible(true);
    }
}

void line_widget::moveCallout(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton) {
        callout->position->setPixelPosition(e->pos());
    }
}

void line_widget::hideCallout(bool toggled)
{
    callout->setVisible(!toggled);
    tracer->setVisible(!toggled);
    arrow->setVisible(!toggled);
}

void line_widget::setPlotMode(int pm)
{
    setPlotMode(static_cast<LV::PlotMode>(pm));
}

void line_widget::setPlotMode(LV::PlotMode pm)
{
    switch (pm) {
    case LV::pmRAW:
        p_getFrame = &FrameWorker::getFrame;
        break;
    case LV::pmDSF:
        p_getFrame = &FrameWorker::getDSFrame;
        break;
    case LV::pmSNR:
        p_getFrame = &FrameWorker::getSNRFrame;
        break;
    }

    frame_handler->setPlotMode(pm);
}

void line_widget::setOverlayPlot(image_t image_type_overlay)
{
    switch (image_type_overlay) {
    case SPATIAL_PROFILE:
        xAxisMax = static_cast<int>(frWidth);
        qcp->xAxis->setLabel("Spatial index");
        p_getLine = &line_widget::getSpatialLine;
        image_type = image_type_overlay;
        break;
    case SPECTRAL_MEAN:
        xAxisMax = static_cast<int>(frHeight);
        qcp->xAxis->setLabel("Spectral index");
        p_getLine = &line_widget::getSpectralMean;
        plotTitle->setText(QString("Spectral Mean of Single Frame"));
        image_type = image_type_overlay;
        break;
    case SPATIAL_MEAN:
        xAxisMax = static_cast<int>(frWidth);
        qcp->xAxis->setLabel("Spatial index");
        p_getLine = &line_widget::getSpatialMean;
        plotTitle->setText(QString("Spatial Mean of Single Frame"));
        image_type = image_type_overlay;
        break;
    case SPECTRAL_PROFILE:
        xAxisMax = static_cast<int>(frHeight);
        qcp->xAxis->setLabel("Spectral index");
        p_getLine = &line_widget::getSpectralLine;
        image_type = image_type_overlay;
        break;
    default:
        xAxisMax = static_cast<int>(frHeight);
        qcp->xAxis->setLabel("Spectral index");
        p_getLine = &line_widget::getSpectralLine;
    }

    handleNewFrame();
}

void line_widget::setDarkMode(bool dm)
{
    if (dm) {
        qcp->graph(0)->setPen(QPen(Qt::lightGray));
        plotTitle->setTextColor(Qt::white);

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

        callout->setColor(Qt::white);
        callout->setPen(QPen(Qt::white));
        callout->setBrush(QBrush(QColor(0x31363B)));
        callout->setSelectedBrush(QBrush(QColor(0x31363B)));
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
}
