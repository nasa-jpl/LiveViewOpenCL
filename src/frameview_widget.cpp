#include "frameview_widget.h"
#include <QSettings>

frameview_widget::frameview_widget(FrameWorker *fw,
                                   image_t image_type,
                                   QSettings *settings,
                                   QWidget *parent) :
        LVTabApplication(fw, parent),
        image_type(image_type),
        count(0), count_prev(0), fps(0)
{
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
        ceiling = float(UINT16_MAX);
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
    qcp->xAxis->setLabel("x");
    qcp->yAxis->setLabel("y");
    qcp->yAxis->setRangeReversed(true);

    colorMap = new QCPColorMap(qcp->xAxis, qcp->yAxis);
    colorMapData = nullptr;

    colorScale = new QCPColorScale(qcp);
    qcp->plotLayout()->addElement(0, 1, colorScale);

    colorScale->setType(QCPAxis::atRight);
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

    if (settings->value(QString("dark"), USE_DARK_STYLE).toInt()) {
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

    crosshairX = new QCPItemRect(qcp);
    crosshairX->setPen(QPen(Qt::white));
    crosshairY = new QCPItemRect(qcp);
    crosshairY->setPen(QPen(Qt::white));

    crosshairX->bottomRight->setCoords(-100, -100);
    crosshairX->topLeft->setCoords(-100, -100);
    crosshairY->bottomRight->setCoords(-100, -100);
    crosshairY->topLeft->setCoords(-100, -100);

    QCheckBox *hideXbox = new QCheckBox("Hide Crosshair", this);
    connect(hideXbox, SIGNAL(toggled(bool)), this, SLOT(hideCrosshair(bool)));
    hideXbox->setFixedWidth(150);

    QHBoxLayout *boxLayout = new QHBoxLayout(this);
    boxLayout->addWidget(zoomBothButton);
    boxLayout->addWidget(zoomXButton);
    boxLayout->addWidget(zoomYButton);
    zoomButtons->setLayout(boxLayout);

    QVBoxLayout *qvbl = new QVBoxLayout(this);
    QHBoxLayout *bottomControls = new QHBoxLayout;
    bottomControls->addWidget(fpsLabel);
    bottomControls->addWidget(hideXbox);
    bottomControls->addWidget(zoomButtons);
    qvbl->addWidget(qcp, 10);
    qvbl->addLayout(bottomControls, 1);

    this->setLayout(qvbl);

    connect(&renderTimer, SIGNAL(timeout()), this, SLOT(handleNewFrame()));
    connect(&fpsclock, SIGNAL(timeout()), this, SLOT(reportFPS()));
    connect(qcp->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(graphScrolledY(QCPRange)));
    connect(qcp->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(graphScrolledX(QCPRange)));
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
    colorScale->setDataRange(QCPRange(floor, ceiling));
}

void frameview_widget::drawCrosshair(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event)
{
    Q_UNUSED(plottable);
    Q_UNUSED(dataIndex);
    double dataX = qcp->xAxis->pixelToCoord(event->pos().x());
    double dataY = qcp->yAxis->pixelToCoord(event->pos().y());
    crosshairX->bottomRight->setCoords(dataX, 0);
    crosshairX->topLeft->setCoords(dataX, frHeight);
    crosshairY->bottomRight->setCoords(0, dataY);
    crosshairY->topLeft->setCoords(frWidth, dataY);
    frame_handler->setCenter(dataX, dataY);
}

void frameview_widget::hideCrosshair(bool hide)
{
    crosshairX->setVisible(!hide);
    crosshairY->setVisible(!hide);
}
