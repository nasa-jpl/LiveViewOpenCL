#ifndef STDDEVFILTER_H
#define STDDEVFILTER_H

#include <math.h>
#include <fstream>
#include <vector>
#include <array>
#include <string>

#include <QDebug>
#include <QtGlobal>
#include <QFile>

#if (__APPLE__ && __MACH__)
    #include "OpenCL/opencl.h"
#else
    #include "CL/cl.h"
#endif

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

    static std::array<float, NUMBER_OF_BINS> getHistBinValues()
    {
        std::array<float, NUMBER_OF_BINS> values;
        cl_float max = log((1 << 16)); // ln 2^16
        cl_float increment = (max - 0) / NUMBER_OF_BINS;
        cl_float acc = 0;
        for (unsigned int i = 0; i < NUMBER_OF_BINS; i++) {
            values[i] = exp(acc) - 1;
            acc += increment;
        }

        return values;
    }

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
    std::array<unsigned int, NUMBER_OF_BINS> zero_buf;
    cl_mem devInputBuffer;
    cl_mem devOutputBuffer;
    cl_mem hist_bins;
    cl_mem devOutputHist;
    cl_command_queue commandQueue;
    cl_program program;
    cl_kernel kernel;

};

#endif // STDDEVFILTER_H
