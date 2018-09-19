#include "histogram_widget.h"

histogram_widget::histogram_widget(FrameWorker *fw, QWidget *parent) :
    LVTabApplication(fw, parent)
{
    histogram = new QCPBars(qcp->xAxis, qcp->yAxis);
    qcp->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    histogram->setName("Histogram of Standard Deviation Image");
    const uint16_t sigma = 0x03C3;
    qcp->xAxis->setLabel(QString::fromUtf16(&sigma, 1));
    qcp->yAxis->setLabel("Spatial Frequency");

    setCeiling(20000);
    setFloor(0);

    std::array<float, NUMBER_OF_BINS> hist_bin_vals = StdDevFilter::getHistBinValues();
    hist_bins = QVector<double>(NUMBER_OF_BINS);
    for (size_t i = 0; i < NUMBER_OF_BINS; i++) {
        hist_bins[i] = double(hist_bin_vals[i]);
    }
    upperRangeBoundX = hist_bins[hist_bins.size() - 1];

    hist_data = QVector<double>(NUMBER_OF_BINS);

    histogram->keyAxis()->setRangeUpper(512);
    histogram->keyAxis()->setRangeLower(1);
    histogram->keyAxis()->setScaleType(QCPAxis::stLogarithmic);
    histogram->valueAxis()->setRange(QCPRange(0, getCeiling()));

    if (fw->settings->value(QString("dark"), USE_DARK_STYLE).toBool()) {
        histogram->setPen(QPen(Qt::lightGray));
        histogram->setBrush(QBrush(QColor("#31363B")));

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
    }

    connect(histogram->keyAxis(), SIGNAL(rangeChanged(QCPRange)), this, SLOT(graphScrolledX(QCPRange)));
    connect(histogram->valueAxis(), SIGNAL(rangeChanged(QCPRange)), this, SLOT(histogramScrolledY(QCPRange)));

    QVBoxLayout *qvbl = new QVBoxLayout(this);
    qvbl->addWidget(qcp);
    this->setLayout(qvbl);

    connect(&renderTimer, SIGNAL(timeout()), this, SLOT(handleNewFrame()));
    renderTimer.start(FRAME_DISPLAY_PERIOD_MSECS);
}

histogram_widget::~histogram_widget()
{
}

void histogram_widget::handleNewFrame()
{
    if (!this->isHidden() && frame_handler->running()) {
        uint32_t *hist_data_ptr = frame_handler->getHistData();
        for (unsigned int b = 0; b < NUMBER_OF_BINS; b++) {
            hist_data[b] = hist_data_ptr[b];
        }
        histogram->setData(hist_bins, hist_data);
        qcp->replot();
    }
}

void histogram_widget::histogramScrolledY(const QCPRange &newRange)
{
    Q_UNUSED(newRange);
    rescaleRange();
}

void histogram_widget::rescaleRange()
{
    qcp->yAxis->setRange(QCPRange(getFloor(), getCeiling()));
}

void histogram_widget::resetRange()
{
    qcp->xAxis->setRange(QCPRange(1, hist_bins[hist_bins.size() - 1]));
}
