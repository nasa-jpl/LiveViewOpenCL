#ifndef CONSTANTS_H
#define CONSTANTS_H

const static unsigned int CPU_FRAME_BUFFER_SIZE = 50;
const static unsigned int MAX_SIZE = 2560*2560;
const static unsigned int MAX_N = 50;
const static unsigned int GPU_FRAME_BUFFER_SIZE = 150;
const static unsigned int BLOCK_SIZE = 20;

const static unsigned int FRAME_SKIP_FACTOR = 10; //This means only every frame modulo 10 will be redrawn, this has to do with the slowness of qcustomplot, a lower value will increase the frame rate. A value of 0 will make it attempt to draw every single frame
const static unsigned int TARGET_FRAMERATE = 60;
const static unsigned int FRAME_DISPLAY_PERIOD_MSECS = 1000 / TARGET_FRAMERATE;


#endif // CONSTANTS_H
