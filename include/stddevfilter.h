#ifndef STDDEVFILTER_H
#define STDDEVFILTER_H

#include <QDebug>
#include <QtGlobal>
#include <fstream>
#include <vector>
#include <string>

#ifdef __APPLE__
    #include "OpenCL/opencl.h"
#else
    #include "CL/cl.h"
#endif

#include "constants.h"

class StdDevFilter
{
public:
    StdDevFilter(int frame_width, int frame_height) :
        gpu_buffer_head(0), frWidth(frame_width),
        frHeight(frame_height), N(MAX_N) {}
    virtual ~StdDevFilter();

    bool start();

private:
    std::string GetPlatformName(cl_platform_id id);
    std::string GetDeviceName(cl_device_id id);
    void CheckError(cl_int error);
    std::string LoadKernel(const char *name);
    cl_program CreateProgram(const std::string &source,
                             cl_context context);

    unsigned int gpu_buffer_head;
    cl_int frWidth;
    cl_int frHeight;
    cl_int N;
    cl_ushort **in_frameset;
    cl_ushort *in_frame;
    cl_float *out_frame;
    cl_float *bins_histogram;
    uint *out_histogram;
    cl_context context;
    cl_device_id *devices;
    cl_mem inputBuffer;
    cl_mem outputBuffer;
    cl_command_queue commandQueue;
    cl_program program;
    cl_kernel kernel;
    cl_ulong availableLocalMem;
    cl_ulong neededLocalMem;



};

#endif // STDDEVFILTER_H
