#include "frameview_widget.h"

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

    QCPMarginGroup *marginGroup = new QCPMarginGroup(qcp);
    qcp->axisRect()->setMarginGroup(QCP::msBottom | QCP::msTop, marginGroup);
    colorScale->setMarginGroup(QCP::msBottom | QCP::msTop, marginGroup);

    setDarkMode();

    qcp->rescaleAxes();
    qcp->axisRect()->setBackgroundScaled(false);

    fpsLabel = new QLabel("Starting...");
    fpsLabel->setFixedWidth(150);

    QComboBox *zoomOptions = new QComboBox();
    zoomOptions->addItem("Zoom on Both axes");
    zoomOptions->addItem("Zoom on X axis only");
    zoomOptions->addItem("Zoom on Y axis only");
    connect(zoomOptions, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=](int index) {
        switch (index) {
        case 0:
            qcp->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
            qcp->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
            break;
        case 1:
            qcp->axisRect()->setRangeZoom(Qt::Horizontal);
            qcp->axisRect()->setRangeDrag(Qt::Horizontal);
            break;
        case 2:
            qcp->axisRect()->setRangeZoom(Qt::Vertical);
            qcp->axisRect()->setRangeDrag(Qt::Vertical);
            break;
        default:
            qcp->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
            qcp->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
        }
    });

    crosshairX = new QCPItemRect(qcp);
    crosshairX->setPen(QPen(Qt::white));
    crosshairY = new QCPItemRect(qcp);
    crosshairY->setPen(QPen(Qt::white));

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

    s_0 = frame_handler->getCenter()->x();
    s_1 = frame_handler->getCenter()->x();

    l_0 = frame_handler->getCenter()->y();
    l_1 = frame_handler->getCenter()->y();

    QPen pen(QColor(230,230,230));
    tlBox->setPen(pen);
    trBox->setPen(pen);
    blBox->setPen(pen);
    brBox->setPen(pen);

    /*QBrush brush(QColor(126, 126, 126, 192));
    tlBox->setBrush(brush);
    trBox->setBrush(brush);
    blBox->setBrush(brush);
    brBox->setBrush(brush);*/

    crosshairX->setPen(QPen(Qt::white));

    QCheckBox *hideXbox = new QCheckBox("Hide Crosshair", this);
    connect(hideXbox, SIGNAL(toggled(bool)), this, SLOT(hideCrosshair(bool)));
    hideXbox->setFixedWidth(150);

    QVBoxLayout *qvbl = new QVBoxLayout(this);
    QHBoxLayout *bottomControls = new QHBoxLayout;
    bottomControls->addWidget(fpsLabel);
    bottomControls->addWidget(hideXbox);

    if (image_type == DSF) { //Dark Sub Widget Only
        QCheckBox *plotModeCheckbox =
                new QCheckBox("Plot Signal-to-Noise Ratio", this);
        connect(plotModeCheckbox, &QCheckBox::toggled,
                this, &frameview_widget::setPlotMode);
        plotModeCheckbox->setFixedWidth(150);
        bottomControls->addWidget(plotModeCheckbox);
    }

    bottomControls->addWidget(zoomOptions);

    qvbl->addWidget(qcp, 10);
    qvbl->addLayout(bottomControls, 1);

    this->setLayout(qvbl);

    connect(&renderTimer, SIGNAL(timeout()), this, SLOT(handleNewFrame()));
    connect(&fpsclock, SIGNAL(timeout()), this, SLOT(reportFPS()));
    connect(qcp->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(graphScrolledY(QCPRange)));
    connect(qcp->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(graphScrolledX(QCPRange)));

    connect(qcp, &QCustomPlot::mousePress, this, &frameview_widget::mouse_down);
    connect(qcp, &QCustomPlot::mouseMove, this, &frameview_widget::mouse_move);
    connect(qcp, &QCustomPlot::mouseRelease, this, &frameview_widget::mouse_up);
    if (image_type == BASE) {
        connect(qcp, SIGNAL(plottableDoubleClick(QCPAbstractPlottable*, int, QMouseEvent*)),
                this, SLOT(drawCrosshair(QCPAbstractPlottable*, int, QMouseEvent*)));
    }
    colorMapData = new QCPColorMapData(frWidth, frHeight, QCPRange(0, frWidth-1), QCPRange(0, frHeight-1));
    colorMap->setData(colorMapData);
    if (frame_handler->running()) {
        renderTimer.start(FRAME_DISPLAY_PERIOD_MSECS);
        fpsclock.start(1000); // 1 sec
    }
}

frameview_widget::~frameview_widget()
{
}

void frameview_widget::handleNewFrame()
{
    if (!this->isHidden() && frame_handler->running()) {

        std::vector<float>image_data{(frame_handler->*p_getFrame)()};
        for (unsigned int col = 0; col < frWidth; col++) {
            for (unsigned int row = 0; row < frHeight; row++ ) {
                colorMap->data()->setCell(col, row, image_data[row * frWidth + col]); // y-axis NOT reversed
            }
        }
        qcp->replot();
        count++;
    }

    // count-based FPS counter, gets slower to update the lower the fps,
    // but can provide fractional fps values.
    /* if (count % 50 == 0 && count != 0) {
        fps = 50.0 / fpsclock.restart() * 1000.0;
        fps_string = QString::number(fps, 'f', 1);
        fpsLabel->setText(QString("Display: %1 fps").arg(fps_string));
    } */
}

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
    if(event->button()== Qt::RightButton) {
        return;
    } else {
        Q_UNUSED(plottable);
        Q_UNUSED(dataIndex);
        double dataX = qcp->xAxis->pixelToCoord(event->pos().x());
        double dataY = qcp->yAxis->pixelToCoord(event->pos().y());
        crosshairX->bottomRight->setCoords(dataX, 0);
        crosshairX->topLeft->setCoords(dataX, frHeight);
        crosshairY->bottomRight->setCoords(0, dataY);
        crosshairY->topLeft->setCoords(frWidth, dataY);
        s_0 += qcp->xAxis->pixelToCoord(event->pos().x()) - frame_handler->getCenter()->x();
        s_1 += qcp->xAxis->pixelToCoord(event->pos().x()) - frame_handler->getCenter()->x();

        l_0 += qcp->yAxis->pixelToCoord(event->pos().y()) - frame_handler->getCenter()->y();
        l_1 += qcp->yAxis->pixelToCoord(event->pos().y()) - frame_handler->getCenter()->y();

        frame_handler->setCenter(dataX, dataY);
        if(boxes_enabled) {
            tlBox->topLeft->setCoords(0, 0);
            tlBox->bottomRight->setCoords(s_0, l_0);
            trBox->topLeft->setCoords(s_1, 0);
            trBox->bottomRight->setCoords(frWidth, l_0);

            blBox->topLeft->setCoords(0, l_1);
            blBox->bottomRight->setCoords(s_0, frHeight);
            brBox->topLeft->setCoords(s_1, l_1);
            brBox->bottomRight->setCoords(frWidth, frHeight);
        }
    }
}

void frameview_widget::hideCrosshair(bool hide)
{
    crosshairX->setVisible(!hide);
    crosshairY->setVisible(!hide);
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
    }
}

void frameview_widget::mouse_move(QMouseEvent *event) {
    if(boxes_enabled) {
        if(dragging_horizontal_box) {
            double limitx = qcp->xAxis->pixelToCoord(event->pos().x());
            if(limitx < frame_handler->getCenter()->x()) {
                s_0 = limitx; //2a - b = a + (a - b)
                s_1 = 2 * frame_handler->getCenter()->x() - limitx; //2a - b = a + (a - b)
            } else {
                s_1 = limitx; //2a - b = a - (b - a)
                s_0 = 2 * frame_handler->getCenter()->x() - limitx; //2a - b = a + (a - b)
            }
        }

        if(dragging_vertical_box) {
            double limity = qcp->yAxis->pixelToCoord(event->pos().y());
            if(limity < frame_handler->getCenter()->y()) {
                l_0 = limity; //2a - b = a + (a - b)
                l_1 = 2 * frame_handler->getCenter()->y() - limity; //2a - b = a + (a - b)
            } else {
                l_1 = limity; //2a - b = a + (a - b)
                l_0 = 2 * frame_handler->getCenter()->y() - limity; //2a - b = a + (a - b)
            }
        }

        tlBox->topLeft->setCoords(0, 0);
        tlBox->bottomRight->setCoords(s_0, l_0);
        trBox->topLeft->setCoords(s_1, 0);
        trBox->bottomRight->setCoords(frWidth, l_0);

        blBox->topLeft->setCoords(0, l_1);
        blBox->bottomRight->setCoords(s_0, frHeight);
        brBox->topLeft->setCoords(s_1, l_1);
        brBox->bottomRight->setCoords(frWidth, frHeight);
    }
}

void frameview_widget::mouse_up(QMouseEvent *event) {
   mouse_move(event);
   dragging_horizontal_box = false;
   dragging_vertical_box = false;
}
