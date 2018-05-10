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
        qFatal("No OpenCL-compatible devices found");
        return false;
    } else {
        qDebug() << "Found" << deviceIdCount << "device(s) across all platforms";
    }

    deviceIds.reserve(deviceIdCount);
    clGetDeviceIDs(platformIds[0], CL_DEVICE_TYPE_ALL, deviceIdCount,
            deviceIds.data(), NULL);

    for (cl_uint i = 0; i < deviceIdCount; i++) {
        qDebug() << "\t (" << (i+1) << ") :" << GetDeviceName(deviceIds[i]).data();
    }

    const cl_context_properties contextProperties [] =
    {
        CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties> (platformIds[0]),
        0, 0
    };

    cl_int error = CL_SUCCESS;
    context = clCreateContext(contextProperties, deviceIdCount,
            deviceIds.data(), NULL, NULL, &error);

    QString build_options("-DGPU_FRAME_BUFFER_SIZE=");
    int fsize = GPU_FRAME_BUFFER_SIZE;
    build_options.append(QString::number(fsize));
    program = CreateProgram(LoadKernel(":kernel/stddev.cl"), context);
    error = clBuildProgram(program, 1, &(deviceIds[2]),
                  build_options.toStdString().data(), 0, NULL);
    if (error != CL_SUCCESS) {
        cl_int errcode;
        size_t build_log_len;
        errcode = clGetProgramBuildInfo(program, deviceIds[2],
                CL_PROGRAM_BUILD_LOG, 0, NULL, &build_log_len);
        if (errcode) {
            qDebug("clGetProgramBuildInfo failed at line %d", __LINE__);
            return false;
        }

        std::vector<char> buff_erro(build_log_len);

        errcode = clGetProgramBuildInfo(program, deviceIds[2],
                CL_PROGRAM_BUILD_LOG, build_log_len, buff_erro.data(), NULL);
        if (errcode) {
            qDebug("clGetProgramBuildInfo failed at line %d", __LINE__);
            return false;
        }

        qDebug("Build log:");
        qDebug() << buff_erro.data();
        qDebug("clBuildProgram failed.");
        return false;
    }

    kernel = clCreateKernel(program, "std_dev_filter_kernel", &error);
    CheckError(error, __LINE__);

    devInputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY,
            frWidth * frHeight * sizeof(cl_ushort) * GPU_FRAME_BUFFER_SIZE, NULL, &error);
    CheckError(error, __LINE__);

    devOutputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
            frWidth * frHeight * sizeof(cl_float), NULL, &error);
    CheckError(error, __LINE__);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &devInputBuffer);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &devOutputBuffer);
    clSetKernelArg(kernel, 2, sizeof(cl_uint), &frWidth);
    clSetKernelArg(kernel, 3, sizeof(cl_uint), &frHeight);
    clSetKernelArg(kernel, 4, sizeof(cl_int), &gpu_buffer_head);
    clSetKernelArg(kernel, 5, sizeof(cl_uint), &N);

    commandQueue = clCreateCommandQueue(context, deviceIds[2], 0, &error);
    CheckError(error, __LINE__);

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

void StdDevFilter::CheckError(cl_int error, int line)
{
    if (error != CL_SUCCESS) {
        const char *errorString = getOpenCLErrorString(error);
        qFatal("OpenCL call failed with error %s on line %d", errorString, line);
        std::exit(1);
    }
}

const char* StdDevFilter::getOpenCLErrorString(cl_int error)
{
    switch(error){
        // run-time and JIT compiler errors
        case 0: return "CL_SUCCESS";
        case -1: return "CL_DEVICE_NOT_FOUND";
        case -2: return "CL_DEVICE_NOT_AVAILABLE";
        case -3: return "CL_COMPILER_NOT_AVAILABLE";
        case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
        case -5: return "CL_OUT_OF_RESOURCES";
        case -6: return "CL_OUT_OF_HOST_MEMORY";
        case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
        case -8: return "CL_MEM_COPY_OVERLAP";
        case -9: return "CL_IMAGE_FORMAT_MISMATCH";
        case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
        case -11: return "CL_BUILD_PROGRAM_FAILURE";
        case -12: return "CL_MAP_FAILURE";
        case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
        case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
        case -15: return "CL_COMPILE_PROGRAM_FAILURE";
        case -16: return "CL_LINKER_NOT_AVAILABLE";
        case -17: return "CL_LINK_PROGRAM_FAILURE";
        case -18: return "CL_DEVICE_PARTITION_FAILED";
        case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

        // compile-time errors
        case -30: return "CL_INVALID_VALUE";
        case -31: return "CL_INVALID_DEVICE_TYPE";
        case -32: return "CL_INVALID_PLATFORM";
        case -33: return "CL_INVALID_DEVICE";
        case -34: return "CL_INVALID_CONTEXT";
        case -35: return "CL_INVALID_QUEUE_PROPERTIES";
        case -36: return "CL_INVALID_COMMAND_QUEUE";
        case -37: return "CL_INVALID_HOST_PTR";
        case -38: return "CL_INVALID_MEM_OBJECT";
        case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
        case -40: return "CL_INVALID_IMAGE_SIZE";
        case -41: return "CL_INVALID_SAMPLER";
        case -42: return "CL_INVALID_BINARY";
        case -43: return "CL_INVALID_BUILD_OPTIONS";
        case -44: return "CL_INVALID_PROGRAM";
        case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
        case -46: return "CL_INVALID_KERNEL_NAME";
        case -47: return "CL_INVALID_KERNEL_DEFINITION";
        case -48: return "CL_INVALID_KERNEL";
        case -49: return "CL_INVALID_ARG_INDEX";
        case -50: return "CL_INVALID_ARG_VALUE";
        case -51: return "CL_INVALID_ARG_SIZE";
        case -52: return "CL_INVALID_KERNEL_ARGS";
        case -53: return "CL_INVALID_WORK_DIMENSION";
        case -54: return "CL_INVALID_WORK_GROUP_SIZE";
        case -55: return "CL_INVALID_WORK_ITEM_SIZE";
        case -56: return "CL_INVALID_GLOBAL_OFFSET";
        case -57: return "CL_INVALID_EVENT_WAIT_LIST";
        case -58: return "CL_INVALID_EVENT";
        case -59: return "CL_INVALID_OPERATION";
        case -60: return "CL_INVALID_GL_OBJECT";
        case -61: return "CL_INVALID_BUFFER_SIZE";
        case -62: return "CL_INVALID_MIP_LEVEL";
        case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
        case -64: return "CL_INVALID_PROPERTY";
        case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
        case -66: return "CL_INVALID_COMPILER_OPTIONS";
        case -67: return "CL_INVALID_LINKER_OPTIONS";
        case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

        // extension errors
        case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
        case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
        case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
        case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
        case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
        case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
        default: return "Unknown OpenCL error";
    }
}

const std::string StdDevFilter::LoadKernel(const char *name)
{
    QFile kernel(name);
    if (!kernel.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Failed to open kernel");
    }

    QTextStream in(&kernel);
    QString result(in.readAll());
    return result.toStdString();
}

cl_program StdDevFilter::CreateProgram(const std::string &source, cl_context context)
{
    size_t lengths[1] = { source.size() };
    const char* sources[1] = { source.data() };

    cl_int error = 0;
    cl_program program = clCreateProgramWithSource(context, 1, sources, lengths, &error);
    CheckError(error, __LINE__);

    return program;
}

void StdDevFilter::compute_stddev(LVFrame *new_frame, cl_uint new_N)
{
    static int count = 0;
    N = new_N;
    size_t devMemOffset = gpu_buffer_head * frWidth * frHeight * sizeof(cl_ushort);
    CheckError(clEnqueueWriteBuffer(commandQueue, devInputBuffer, CL_FALSE, devMemOffset, frWidth * frHeight * sizeof(cl_ushort),
                         new_frame->raw_data, 0, NULL, NULL), __LINE__);
    size_t offset[3] = { 0 };
    size_t work_size[3] = { frWidth, frHeight, N };
    size_t max_work_size;
    CheckError(clGetDeviceInfo(deviceIds[2], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &max_work_size, NULL), __LINE__);
    double work_dim = sqrt(max_work_size);
    size_t local_work_size[2] = { (size_t)work_dim, (size_t)work_dim };
    CheckError(clEnqueueNDRangeKernel(commandQueue, kernel, 2,
                                      offset, work_size, local_work_size,
                                      0, NULL, NULL), __LINE__);
    CheckError(clEnqueueReadBuffer(commandQueue, devOutputBuffer, CL_FALSE, 0, frWidth * frHeight * sizeof(cl_float),
                                   new_frame->sdv_data, 0, NULL, NULL), __LINE__);

    if (++gpu_buffer_head == GPU_FRAME_BUFFER_SIZE) {
        gpu_buffer_head = 0;
    }
    if (currentN < MAX_N) {
        currentN++;
    }
    count++;

}
