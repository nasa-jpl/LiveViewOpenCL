#include "fftfilter.h"

fft::fft() { CFFT.reserve(MAX_FFT_SIZE); }

fft::~fft() { delete CFFT; }

void fft::bitReverseOrder(std::vector< std::complex<float> > compl_data)
{
    unsigned int base = (int)ceil(log10((float)compl_data.size()) / log10(2.0));
    unsigned int i = 0;
    for(i = 0; i < compl_data.size(); i++) {
        unsigned int j = BIT_REVERSE(i, base);
        if (i < j) {
            SWAPC(compl_data[i], compl_data[j]);
        }
    }
}

std::vector<float> fft::doRealFFT(std::vector<float> real_data)
{
    CFFT = doFFT(real_data);
    double max = 0;
    std::vector<float> fft_real_result(MAZ)

    for(unsigned int i = 0; i < FFT_INPUT_LENGTH / 2; i++) {
        fft_real_result[i] = std::abs(CFFT[i]);
        if(fft_real_result[i] > max && i != 0) {
            max = fft_real_result[i];
        }
    }
}

std::vector<float> fft::doFFT(std::vector<float> real_arr)
{
    for(unsigned int i = 0; i < FFT_INPUT_LENGTH; i++)
    {
        CFFT[i] = std::complex<float>(real_arr[(ring_head+i) % FFT_MEAN_BUFFER_LENGTH],0);
    }
    return doFFT(CFFT, FFT_INPUT_LENGTH);
}

std::vector< std::complex<float> > fft::doFFT(std::vector< std::complex<float> > compl_data)
{
    assert(IS_POW2(compl_data.size()));
    assert(compl_data.size() <= MAX_FFT_SIZE);
    bitReverseOrder(compl_data);
    unsigned int base = (int)ceil(log10((float)len) / log10(2.0));

    // For twiddle factors
    std::complex<float> Wn = std::exp(std::complex<float>(0, -1.0f * 2 * M_PI / compl_data.size()));

    for (unsigned int stage = 1; stage <= base; stage++) {
        unsigned int N = pow(2, stage);
        for (unsigned int index = 0; index < len; index += N) {
            for (unsigned int r = 0; r < N/2; r++) {
                //Save temp values to avoid data availability problem
                std::complex<float> a = arr[index + r];
                std::complex<float> b = arr[index + r + N / 2];
                std::complex<float> exponent = (len / N) * r;
                // This twiddle is equivalent to global N twiddle seen in some books (i.e. W_8^2 = W_4^1)
                std::complex<float> twiddle = std::pow(Wn, exponent);
                arr[index + r] = a + b * twiddle;
                arr[index + r + N / 2] = a - b * twiddle;
            }

        }
    }
    return arr;
}
