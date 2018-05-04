#ifndef CONSTANTS_H
#define CONSTANTS_H

#define CPU_FRAME_BUFFER_SIZE 10
#define MAX_SIZE 2560*2560
#define MAX_N 500
#define GPU_FRAME_BUFFER_SIZE 1500
#define BLOCK_SIZE 20

#define FRAME_SKIP_FACTOR 10 //This means only every frame modulo 10 will be redrawn, this has to do with the slowness of qcustomplot, a lower value will increase the frame rate. A value of 0 will make it attempt to draw every single frame
#define TARGET_FRAMERATE  60
#define FRAME_DISPLAY_PERIOD_MSECS 1000 / TARGET_FRAMERATE


#endif // CONSTANTS_H
