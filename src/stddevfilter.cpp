#include "stddevfilter.h"

StdDevFilter::~StdDevFilter()
{

}

bool StdDevFilter::start()
{
    cl_uint platformIdCount = 0;
    clGetPlatformIDs(0, NULL, &platformIdCount);
    if (platformIdCount == 0) {
        qFatal("No OpenCL platform found!");
        return false;
    } else {
        qDebug() << "Found" << (int)platformIdCount << " platform(s)";
    }

    std::vector<cl_platform_id> platformIds(platformIdCount);
    clGetPlatformIDs(platformIdCount, platformIds.data(), NULL);

    for (cl_uint i = 0; i < platformIdCount; ++i) {
        qDebug() << "\t (" << (i+1) << ") :" << GetPlatformName(platformIds[i]).data();
    }

    cl_uint deviceIdCount = 0;
    clGetDeviceIDs(platformIds[0], CL_DEVICE_TYPE_ALL, 0, NULL,
            &deviceIdCount);
    if (deviceIdCount == 0) {
        qFatal("No OpenCL devices found!");
        return false;
    } else {
        qDebug() << "Found" << (int)deviceIdCount << "device(s)";
    }

    std::vector<cl_device_id> deviceIds(deviceIdCount);
    clGetDeviceIDs(platformIds[0], CL_DEVICE_TYPE_ALL, deviceIdCount,
            deviceIds.data(), NULL);

    for (cl_uint i = 0; i < deviceIdCount; ++i) {
        qDebug() << "\t (" << (i+1) << ") :" << GetDeviceName(deviceIds[i]).data();
    }

    const cl_context_properties contextProperties [] =
    {
        CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties> (platformIds[0]),
        0, 0
    };

    cl_int error = CL_SUCCESS;
    cl_context context = clCreateContext(contextProperties, deviceIdCount,
            deviceIds.data(), NULL, NULL, &error);

    program = CreateProgram(LoadKernel("kernels/stddev.cl"), context);
    /* CheckError(clBuildProgram(program, deviceIdCount, deviceIds.data(),
                  "-I./include/", NULL, NULL)); */

    // kernel = clCreateKernel(program, "StdDev", &error);

    qDebug("Context created");
    return true;
}

std::string StdDevFilter::GetPlatformName(cl_platform_id id)
{
    size_t size = 0;
    clGetPlatformInfo(id, CL_PLATFORM_NAME, 0, NULL, &size);

    std::string result;
    result.resize(size);
    clGetPlatformInfo(id, CL_PLATFORM_NAME, size,
            const_cast<char*>(result.data()), NULL);

    return result;
}

std::string StdDevFilter::GetDeviceName(cl_device_id id)
{
    size_t size = 0;
    clGetDeviceInfo(id, CL_DEVICE_NAME, 0, NULL, &size);

    std::string result;
    result.resize(size);
    clGetDeviceInfo(id, CL_DEVICE_NAME, size,
            const_cast<char*>(result.data()), NULL);

    return result;
}

void StdDevFilter::CheckError(cl_int error)
{
    if (error != CL_SUCCESS) {
        qFatal("OpenCL call failed with error %d", (int)error);
        std::exit(1);
    }
}

std::string StdDevFilter::LoadKernel(const char *name)
{
    std::ifstream in(name);
    std::string result((std::istreambuf_iterator<char>(in)),
                       std::istreambuf_iterator<char>());
    return result;
}

cl_program StdDevFilter::CreateProgram(const std::string &source, cl_context context)
{
    size_t lengths[1] = { source.size() };
    const char* sources[1] = { source.data() };

    cl_int error = 0;
    cl_program program = clCreateProgramWithSource(context, 1, sources, lengths, &error);
    CheckError(error);

    return program;
}
