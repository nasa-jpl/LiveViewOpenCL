#ifndef STDDEVFILTER_H
#define STDDEVFILTER_H

#include <math.h>
#include <fstream>
#include <vector>
#include <string>

#include <QDebug>
#include <QtGlobal>
#include <QFile>

#ifdef __APPLE__
    #include "OpenCL/opencl.h"
#else
    #include "CL/cl.h"
#endif

#include <QFile>

#include "constants.h"
#include "lvframe.h"

class StdDevFilter
{
public:
    StdDevFilter(unsigned int frame_width, unsigned int frame_height, cl_uint _N) :
        gpu_buffer_head(0), frWidth(frame_width),
        frHeight(frame_height), N(_N) {}
    virtual ~StdDevFilter();

    bool start();

    void compute_stddev(LVFrame *new_frame, cl_uint new_N);

private:
    std::string GetPlatformName(cl_platform_id id);
    std::string GetDeviceName(cl_device_id id);
    void CheckError(cl_int error, int line);
    const char* getOpenCLErrorString(cl_int error);
    const std::string LoadKernel(const char *name);
    cl_program CreateProgram(const std::string &source,
                             cl_context context);

    cl_int gpu_buffer_head;
    cl_uint frWidth;
    cl_uint frHeight;
    cl_uint N;
    cl_uint currentN;
    cl_context context;
    std::vector<cl_device_id> deviceIds;
    cl_mem devInputBuffer;
    cl_mem devOutputBuffer;
    cl_command_queue commandQueue;
    cl_program program;
    cl_kernel kernel;

};

#endif // STDDEVFILTER_H
