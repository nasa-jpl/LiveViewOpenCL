#include "overlay_widget.h"
#include "constants.h"
/* #define QDEBUG */

overlay_widget::overlay_widget(FrameWorker *fw, image_t image_t, QWidget *parent, QSettings *settings) :
    LVTabApplication(fw, parent), image_type(image_t), settings(settings)
{
    /*! \brief Establishes a plot for a specified image type.
     * \param image_type Determines the type of graph that will be output by profile_widget
     * \author Jackie Ryan
     * \author Noah Levy */
    
    //qcp = NULL;
    //this->fw = fw;
    //this->me = me;
    ceiling = UINT16_MAX;
    floor = 0;
    //frHeight = fw->getFrameHeight();
    //frWidth = fw->getFrameWidth();
    x_coord = -1;
    y_coord = -1;

    qcp = new QCustomPlot(this);
    //qcp->addLayer("Plot Layer");
    //qcp->setCurrentLayer("Plot Layer");
    //qcp->setAntialiasedElement(QCP::aeAll);

    qcp->addGraph(nullptr, nullptr);


    topWidget = new frameview_widget(fw, DSF, fw->settings);
    bottomWidget = new line_widget(fw, SPATIAL_PROFILE);

    widgetLayout = new QHBoxLayout(this);
    widgetLayout->addWidget(topWidget);
    widgetLayout->addWidget(bottomWidget);
    this->setLayout(widgetLayout);

    p_getFrame = &FrameWorker::getFrame;

    upperRangeBoundX = xAxisMax;

    x = QVector<double>(xAxisMax);
    for (int i = 0; i < xAxisMax; i++) {
        x[i] = double(i);
    }

    y = QVector<double>(xAxisMax);
    y_lh = QVector<double>(xAxisMax);

    qcp->xAxis->setRange(QCPRange(0, xAxisMax));

    qcp->setInteractions(QCP::iRangeZoom | QCP::iSelectItems);

    qcp->yAxis->setLabel("Pixel Magnitude [DN]");
    setCeiling((1<<16) -1);
    setFloor(0);

    qcp->graph(0)->setData(x, y);
    //qcp->graph(1)->setData(x, y);

    qcp->addLayer("Box Layer", qcp->currentLayer());
    qcp->setCurrentLayer("Box Layer");

    callout = new QCPItemText(qcp);
    callout->position->setCoords(xAxisMax / 2, ceiling - 1000);
    callout->setFont(QFont(font().family(), 16));
    callout->setPen(QPen(Qt::black));
    callout->setBrush(Qt::white);
    qcp->setSelectionTolerance(100);
    callout->setSelectedBrush(Qt::white);
    callout->setSelectedFont(QFont(font().family(), 16));
    callout->setSelectedPen(QPen(Qt::black));
    callout->setSelectedColor(Qt::black);
    callout->setVisible(false);

    qcp->addLayer("Arrow Layer", qcp->currentLayer(), QCustomPlot::limBelow);
    qcp->setCurrentLayer("Arrow Layer");

    arrow = new QCPItemLine(qcp);
    arrow->start->setParentAnchor(callout->bottom);
    arrow->setHead(QCPLineEnding::esSpikeArrow);
    arrow->setSelectable(false);
    arrow->setVisible(false);
    qcp->setInteractions(QCP::iRangeZoom | QCP::iSelectItems | QCP::iRangeDrag);

    setDarkMode(fw->settings->value(QString("dark"), USE_DARK_STYLE).toBool());

    qcp->yAxis->setLabel("Pixel Magnitude [DN]");
    qcp->yAxis->setRange(QCPRange(0, UINT16_MAX)); //From 0 to 2^16

    qcp->graph(0)->setData(x, y);

    plotModeBox = new QComboBox();
    plotModeBox->addItem("Raw Data");
    plotModeBox->addItem("Dark Subtracted Data");
    plotModeBox->addItem("Signal-to-Noise Ratio Data");
    connect(plotModeBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setPlotMode(int)));

    auto bottomButtons = new QHBoxLayout;
    //bottomButtons->addWidget(hideTracer);
    bottomButtons->addWidget(plotModeBox);

    auto qvbl = new QVBoxLayout(this);
    qvbl->addWidget(qcp);
    qvbl->addLayout(bottomButtons);
    this->setLayout(qvbl);

    showCalloutCheck = new QCheckBox("Display Callout");
    showCalloutCheck->setChecked(false);

    zoomX_enable_Check = new QCheckBox("Enable X-Zoom");
    zoomX_enable_Check->setChecked(true);

    zoomY_enable_Check = new QCheckBox("Enable Y-Zoom");
    zoomY_enable_Check->setChecked(true);

    spacer = new QSpacerItem(50,1,QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    spacer->setAlignment(Qt::AlignRight);

    reset_zoom_btn = new QPushButton("Reset Zoom");
    reset_zoom_btn->setToolTip("Reset the X-axis to full-frame, and the y-axis to full-scale.\n If displaying dark-subtracted data, the y-axis goes to +/- 200 DN.");


    /*if(itype==VERT_OVERLAY)
    {
        overlay_img = new frameview_widget(fw, DSF, this);

        // Grid layout

        // Right side plot and check box:
        op_vert.addWidget(qcp,1);
        // op_vert.addWidget(showCalloutCheck, 2);

        // Assemble Left and Right together:
        // qgl.addWidget(qcp, 1,2,1,1); // profile plot
        // qgl.addWidget(showCalloutCheck, 2,2,1,1); //  "Show Callout"

        horiz_layout.addSpacerItem(spacer);
        horiz_layout.addWidget(showCalloutCheck,0);
        horiz_layout.addWidget(zoomX_enable_Check,0);
        horiz_layout.addWidget(zoomY_enable_Check,0);
        horiz_layout.addWidget(reset_zoom_btn,0);

        op_vert.addLayout(&horiz_layout,1);

        // Place vertical layout on right side:
        qgl.addLayout(&op_vert, 0,2, Qt::AlignBaseline);


        // Left side frame view image:
        qgl.addWidget(overlay_img, 0,1,1,1); // frame view, dark subtracted

        //TODO: Zoom-X, Zoom-Y toggles for plot
        this->setLayout(&qgl);
    } else {
        // VBox layout
        qvbl.addWidget(qcp);

        horiz_layout.addWidget(showCalloutCheck,0);
        horiz_layout.addWidget(zoomX_enable_Check,0);
        horiz_layout.addWidget(zoomY_enable_Check,0);
        horiz_layout.addWidget(reset_zoom_btn,0);

        horiz_layout.addSpacerItem(spacer);

        qvbl.addLayout(&horiz_layout, 1);
        qvbl.addWidget(showCalloutCheck);

        this->setLayout(&qvbl);
    }*/

    /*connect(reset_zoom_btn, SIGNAL(released()), this, SLOT(defaultZoom())); // disconnect?
    connect(qcp, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(moveCallout(QMouseEvent*)));
    connect(qcp, SIGNAL(mouseDoubleClick(QMouseEvent*)), this, SLOT(setCallout(QMouseEvent*)));
    connect(qcp->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(profileScrolledX(QCPRange)));
    connect(qcp->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(profileScrolledY(QCPRange)));
    connect(showCalloutCheck, SIGNAL(clicked()), this, SLOT(hideCallout()));
    connect(&rendertimer, SIGNAL(timeout()), this, SLOT(handleNewFrame()));
    //connect(frame_handler, &FrameWorker::crosshairChanged, this, &line_widget::updatePlotTitle);
    
    connect(frame_handler->Camera, &CameraModel::timeout, this, [=]() {
        QVector<double> zero_data(x.length(), 0);
        //qcp->graph(0)->setData(x, zero_data);
        qcp->replot();
        // renderTimer.stop();
    });

    if (frame_handler->running()) {
        renderTimer.start(FRAME_DISPLAY_PERIOD_MSECS);
    }

    rendertimer.start(FRAME_DISPLAY_PERIOD_MSECS);*/
}
overlay_widget::~overlay_widget()
{
    delete overlay_img;
}

QVector<double> overlay_widget::getSpectralLine(QPointF coord)
{
    QVector<double> graphData(static_cast<int>(frHeight));
    auto col = int(coord.y());
    std::vector<float> image_data = (frame_handler->*p_getFrame)();
    for (int r = 0; r < frHeight; r++) {
        graphData[r] = static_cast<double>(image_data[size_t(r * frWidth + col)]);
    }
    return graphData;
}

QVector<double> overlay_widget::getSpatialLine(QPointF coord)
{
    QVector<double> graphData(static_cast<int>(frWidth));
    int row = int(coord.x());
    std::vector<float> image_data = (frame_handler->*p_getFrame)();
    for (int c = 0; c < frWidth; c++) {
        graphData[c] = static_cast<double>(image_data[size_t(row * frWidth + c)]);
    }
    return graphData;
}

QVector<double> overlay_widget::getSpectralMean(QPointF coord)
{
    Q_UNUSED(coord);
    QVector<double> graphData(static_cast<int>(frHeight));
    float *mean_data = frame_handler->getSpectralMean();
    for (int r = 0; r < frHeight; r++) {
        graphData[r] = static_cast<double>(mean_data[r]);
    }
    return graphData;
}

QVector<double> overlay_widget::getSpatialMean(QPointF coord)
{
    Q_UNUSED(coord);
    QVector<double> graphData(static_cast<int>(frWidth));
    float *mean_data = frame_handler->getSpatialMean();
    for (int c = 0; c < frWidth; c++) {
        graphData[c] = static_cast<double>(mean_data[c]);
    }
    return graphData;
}

// public functions
double overlay_widget::getFloor()
{
    /*! \brief Return the value of the floor for this widget as a double */
    return floor;
}
double overlay_widget::getCeiling()
{
    /*! \brief Return the value of the ceiling for this widget as a double */
    return ceiling;
}

// public slots
void overlay_widget::handleNewFrame()
{
    /*! \brief Plots a specific dimension profile.
     * \paragraph
     * The switch statement is a bit silly here, I only use it to differentiate the plot title and the type of profile array to use.
     * The y-axis data is reversed in these images.
     * \author Jackie Ryan
     */
    
    if (!this->isHidden() && frame_handler->running()) {
        QPointF *center = frame_handler->getCenter();
        if (image_type == SPECTRAL_MEAN || image_type == SPATIAL_MEAN || center->x() > -0.1) {
            y = (this->*p_getOverlay)(*center);
            //qcp->graph(0)->setData(x, y);
            // replotting is slow when the data set is chaotic... TODO: develop an optimization here
            qcp->replot();
            /*if (!hideTracer->isChecked()) {
                callout->setText(QString(" x: %1 \n y: %2 ").arg(static_cast<int>(tracer->graphKey()))
                             .arg(y[static_cast<int>(tracer->graphKey())]));
            }*/
        }
    }

        /*case VERT_OVERLAY:
            local_image_ptr = fw->curFrame->vertical_mean_profile; // vertical profiles
            for (int r = 0; r < frHeight; r++)
            {
                y[r] = double(local_image_ptr[r]);
                y_lh[r] = double(fw->curFrame->vertical_mean_profile_lh[r]);
                y_rh[r] = double(fw->curFrame->vertical_mean_profile_rh[r]);

            }
            // display overlay
            qcp->graph(1)->setData(x, y_lh);
            qcp->graph(2)->setData(x, y_rh);
            break;*/

	// display x and y:
        //qcp->graph(0)->setData(x, y);
        qcp->replot();

        if (callout->visible())
            updateCalloutValue();
        switch (image_type) {
        //case SPATIAL_MEAN: plotTitle->setText(QString("Horizontal Mean Profile")); break;
        //case HORIZONTAL_CROSS: plotTitle->setText(QString("Horizontal Profile centered @ y = %1").arg(fw->crosshair_y)); break;
        //case SPECTRAL_MEAN: plotTitle->setText(QString("Vertical Mean Profile")); break;
        //case VERTICAL_CROSS: plotTitle->setText(QString("Vertical Profile centered @ x = %1").arg(fw->crosshair_x)); break;
        //case VERT_OVERLAY: plotTitle->setText(QString("Vertical Overlay")); break; // TODO: Add useful things here
        default: break;
        }
    //} else {
        //plotTitle->setText("No Crosshair designated");
        allow_callouts = false;
        //qcp->graph(0)->clearData();
        qcp->replot();
    //}
    boundedRange_y = qcp->yAxis->range();
    boundedRange_x = qcp->xAxis->range();
}
//}

void overlay_widget::updateCeiling(int c)
{
    /*! \brief Change the value of the ceiling for this widget to the input parameter and replot the color scale. */
    ceiling = (double)c;
    rescaleRange();
}

void overlay_widget::updateFloor(int f)
{
    /*! \brief Change the value of the floor for this widget to the input parameter and replot the color scale. */
    floor = (double)f;
    rescaleRange();
}

void overlay_widget::rescaleRange()
{
    /*! \brief Set the color scale of the display to the last used values for this widget */
    qcp->yAxis->setRange(QCPRange(floor, ceiling));
}

void overlay_widget::profileScrolledX(const QCPRange &newRange)
{
    /*! \brief Controls the behavior of zooming the plot.
     * \param newRange Unused.
     * Profiles must not allow the user to zoom in the x direction.
     */

    if(zoomX_enable_Check->isChecked())
    {
        // Compute intelligent zoom parameters
        // Q_UNUSED(newRange);

        QCPRange boundedRange = newRange;
        // LIL_MIN, BIG_MIN, these are based on pixel amplitude range, not frame geometry.
        double lowerRangeBound = 0;
        double upperRangeBound = xAxisMax;
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
        // floor = boundedRange.lower;
        // ceiling = boundedRange.upper;
        // old:
        // qcp->xAxis->setRange(0, xAxisMax);
        qcp->xAxis->setRange(boundedRange);
        boundedRange_x = boundedRange;
    } else {
        qcp->xAxis->setRange(boundedRange_x);
    }

}
void overlay_widget::profileScrolledY(const QCPRange &newRange)
{
    /*! \brief Controls the behavior of zooming the plot.
     * \param newRange Mouse wheel scrolled range.
     * Profiles must not allow the user to zoom past the dimensions of the frame.
     */

    if(zoomY_enable_Check->isChecked())
    {
        QCPRange boundedRange = newRange;
        double lowerRangeBound = slider_low_inc ? LIL_MIN : BIG_MIN;
        double upperRangeBound = slider_low_inc ? LIL_MAX : BIG_MAX;
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
        floor = boundedRange.lower;
        ceiling = boundedRange.upper;
        qcp->yAxis->setRange(boundedRange);
        boundedRange_y = boundedRange;

    } else {
        qcp->yAxis->setRange(boundedRange_y);
    }
}

void overlay_widget::defaultZoom()
{
    // Set default zoom ("zoom reset" button)
    QCPRange boundedRange_vert;

    // if(dark_sub_enabled)
    /*if(fw->usingDSF())
    {
        boundedRange_vert.lower = -200;
        boundedRange_vert.upper = 200;
    } else {
        boundedRange_vert.lower = 0;
        boundedRange_vert.upper = 65535;
    }*/

    QCPRange boundedRange_horiz;
    boundedRange_horiz.lower = 0;
    boundedRange_horiz.upper = xAxisMax;

    qcp->yAxis->setRange(boundedRange_vert);
    qcp->xAxis->setRange(boundedRange_horiz);

    //TODO: update slider

}

void overlay_widget::setCallout(QMouseEvent *e)
{
    x_coord = qcp->xAxis->pixelToCoord(e->pos().x());
    x_coord = x_coord < 0 ? 0 : x_coord;
    x_coord = x_coord > xAxisMax ? xAxisMax : x_coord;
    y_coord = y[x_coord];
    if (callout->position->coords().y() > ceiling || callout->position->coords().y() < floor)
        callout->position->setCoords(callout->position->coords().x(), (ceiling - floor) * 0.9 + floor);
    callout->setText(QString(" x: %1 \n y: %2 ").arg(x_coord).arg(y_coord));
    if(allow_callouts) {
        arrow->end->setCoords(x_coord, y_coord);
        callout->setVisible(true);
        arrow->setVisible(true);
    }
    showCalloutCheck->setChecked(callout->visible());
}
void overlay_widget::moveCallout(QMouseEvent *e)
{
    // Note, e->posF() was used for previous QT Library versions.
    if ((callout->selectTest(e->pos(), true) < (0.99 * qcp->selectionTolerance())) && (e->buttons() & Qt::LeftButton)) {
        callout->position->setPixelPosition(e->pos());
    } else {
        return;
    }

}
void overlay_widget::hideCallout()
{
    if (callout->visible() || !allow_callouts) {
        callout->setVisible(false);
        arrow->setVisible(false);
    } else {
        callout->setVisible(true);
        arrow->setVisible(true);
    }
    showCalloutCheck->setChecked(callout->visible());
}

void overlay_widget::setDarkMode(bool dm)
{
    if (dm) {
        //qcp->graph(0)->setPen(QPen(Qt::lightGray));
        //plotTitle->setTextColor(Qt::white);

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

// private slots
void overlay_widget::updateCalloutValue()
{
    y_coord = y[x_coord];
    arrow->end->setCoords(x_coord, y_coord);
    callout->setText(QString(" x: %1 \n y: %2 ").arg(x_coord).arg(y_coord));
}

