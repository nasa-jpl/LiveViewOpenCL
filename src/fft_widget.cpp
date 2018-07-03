#include "fft_widget.h"

fft_widget::fft_widget(FrameWorker *fw, QWidget *parent) :
    LVTabApplication(fw, parent)
{
    DCMaskBox = new QCheckBox(QString("Mask DC component"), this);
    DCMaskBox->setChecked(true);
    qcp->xAxis->setLabel("Frequency [Hz]");
    qcp->yAxis->setLabel("Magnitude");

    fft_bars = new QCPBars(qcp->xAxis, qcp->yAxis);
    fft_bars->setName("Magnitude of FFT for Mean Frame Pixel Value");

    freq_bins = QVector<double>(FFT_INPUT_LENGTH / 2);
    rfft_data_vec = QVector<double>(FFT_INPUT_LENGTH / 2);

    QGridLayout *qgl = new QGridLayout(this);
    qgl->addWidget(qcp, 0, 0, 8, 8);
    qgl->addWidget(DCMaskBox, 8, 0, 1, 2);
    this->setLayout(qgl);

    connect(&renderTimer, &QTimer::timeout, this, &fft_widget::handleNewFrame);
    renderTimer.start(FRAME_DISPLAY_PERIOD_MSECS);
}

fft_widget::~fft_widget() {}

void fft_widget::handleNewFrame()
{
    if (!this->isHidden()) {
        double nyquist_freq =  500.0 / FRAME_PERIOD_MS;
        double increment = nyquist_freq / (FFT_INPUT_LENGTH / 2);
        fft_bars->setWidth(increment);

        for (unsigned int i = 0; i < FFT_INPUT_LENGTH / 2; i++) {
            freq_bins[i] = increment * i;
        }

        float *fft_data_ptr = frame_handler->getFrameFFT();
        for (unsigned int b = 0; b < FFT_INPUT_LENGTH / 2; b++) {
            rfft_data_vec[b] = fft_data_ptr[b];
        }
        if (DCMaskBox->isChecked()) {
            rfft_data_vec[0] = 0;
        }
        fft_bars->setData(freq_bins, rfft_data_vec);
        qcp->xAxis->setRange(QCPRange(0, nyquist_freq));
        qcp->replot();
    }
}

void fft_widget::rescaleRange()
{
    qcp->yAxis->setRange(QCPRange(floor, ceiling));
}
