#ifndef CONSTANTS_H
#define CONSTANTS_H

#if (__APPLE__ && __MACH__)
static const bool USE_DARK_STYLE = true;
#else
static const bool USE_DARK_STYLE = false; //dark style does not display some widgets properly on Linux
#endif
static const unsigned int CPU_FRAME_BUFFER_SIZE = 200;
static const unsigned int MAX_SIZE = 2560*2560;
static const int MAX_N = 50;
static const unsigned int GPU_FRAME_BUFFER_SIZE = MAX_N;
static const unsigned int BLOCK_SIZE = 20;
static const int NUMBER_OF_BINS = 1024;

// static const unsigned int FRAME_SKIP_FACTOR = 10; //This means only every frame modulo 10 will be redrawn, this has to do with the slowness of qcustomplot, a lower value will increase the frame rate. A value of 0 will make it attempt to draw every single frame
static const long long FRAME_PERIOD_MS = 40;
// static const unsigned int TARGET_FRAMERATE = 60;
// static const unsigned int FRAME_DISPLAY_PERIOD_MSECS = 1000 / TARGET_FRAMERATE;
static const unsigned int FRAME_DISPLAY_PERIOD_MSECS = 25;

static const unsigned int MAX_FFT_SIZE = 4096;
static const int FFT_INPUT_LENGTH = 512; // Must be 256, will fail silently otherwise
static const unsigned int FFT_MEAN_BUFFER_LENGTH = 512;

static const int CHUNK_NUMLINES = 32;

namespace LV {
    enum PlotMode { pmRAW, pmDSF, pmSNR };
}

#endif // CONSTANTS_H
