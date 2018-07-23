#include "stddevfilter.h"

StdDevFilter::~StdDevFilter()
{
    clReleaseMemObject(devInputBuffer);
    clReleaseMemObject(devOutputBuffer);
    clReleaseMemObject(hist_bins);
    clReleaseMemObject(devOutputHist);
    clReleaseCommandQueue(commandQueue);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    for (auto &ctx : context) {
        clReleaseContext(ctx);
    }
}

bool StdDevFilter::start()
{
    // Find the available platforms on the host computer system.
    // Typically there is only one platform, but some systems have several depending on hardware configuration.
    cl_uint platformIdCount = 0;
    clGetPlatformIDs(0, NULL, &platformIdCount);
    if (platformIdCount == 0) {
        qFatal("No OpenCL platform found!");
        return false;
    } else {
        qDebug() << "Found" << (int)platformIdCount << " platform(s)";
    }

    // List the platform names.
    std::vector<cl_platform_id> platformIds(platformIdCount);
    clGetPlatformIDs(platformIdCount, platformIds.data(), NULL);

    for (cl_uint i = 0; i < platformIdCount; ++i) {
        qDebug() << "\t (" << (i+1) << ") :" << GetPlatformName(platformIds[i]).data();
    }

    // Get the number of devices across all platforms, also counting the number of
    // devices on each platform. The latter aids with associating the device count
    // with the platform while keeping the devices in one list.
    cl_uint deviceIdCount = 0;
    cl_uint totalDeviceCount = 0;
    for (auto &platform : platformIds) {
        clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL,
                    &deviceIdCount);
        devicesPerPlatform.push_back(deviceIdCount);
        totalDeviceCount += deviceIdCount;
    }

    if (totalDeviceCount == 0) {
        qFatal("No OpenCL-compatible devices found on any platform!"); // will abort
        return false;
    } else {
        qDebug() << "Found" << totalDeviceCount << "device(s) across all platforms";
    }

    deviceIds.resize(totalDeviceCount);
    for (cl_uint i = 0; i < platformIdCount; i++) {
        // This is the clunky way that devices can be added to the deviceIds list without associating a device with
        // its platform. This enables multi-platform systems to leave the platform of a device opaque to the user.
        cl_uint ndx = 0;
        for (auto it = devicesPerPlatform.begin(); it != devicesPerPlatform.end() - (platformIdCount - i); ++it) {
            ndx += *it;
        }
        clGetDeviceIDs(platformIds[i], CL_DEVICE_TYPE_ALL, devicesPerPlatform[i],
                &deviceIds[ndx], NULL);
    }

    // Get the types of all devices. TODO: icons on the Computation ModelView for each device type?
    std::vector<cl_device_type> deviceTypes(totalDeviceCount);
    for (cl_uint i = 0; i < totalDeviceCount; i++) {
        qDebug() << "\t (" << (i+1) << ") :" << GetDeviceName(deviceIds[i]).data();
        clGetDeviceInfo(deviceIds[i], CL_DEVICE_TYPE, sizeof(cl_device_type), &deviceTypes[i], NULL);
    }

    // use a reverse iterator to search for the last occurence of a GPU-type device on the device list
    ptrdiff_t pos = std::find(deviceTypes.rbegin(), deviceTypes.rend(), CL_DEVICE_TYPE_GPU) - deviceTypes.rbegin();
    if ((size_t)pos >= deviceTypes.size()) {
        // GPU not found on the system; fall back to using a CPU-type device.
        pos = std::find(deviceTypes.rbegin(), deviceTypes.rend(), CL_DEVICE_TYPE_CPU) - deviceTypes.rbegin();
        if ((size_t)pos >= deviceTypes.size()) {
            qFatal("No suitable (CPU or GPU) OpenCL devices found."); // will abort
            return false;
        }
        // since pos is the result of a reverse iterator, the index in the forward list is the opposite
        device_num = (ptrdiff_t)deviceTypes.size() - pos - 1;
    } else {
        device_num = (ptrdiff_t)deviceTypes.size() - pos - 1;
    }
    platform_num = getPlatformNum(device_num);


    qDebug() << "Using" << GetDeviceName(deviceIds[device_num]).data() << "for some computations. Enter the Computation menu to change.";

    cl_int error = CL_SUCCESS;

    // Generate a context for all devices on each platform
    for (size_t p = 0; p < platformIdCount; p++) {
        const cl_context_properties contextProperties [] =
        {
            CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties> (platformIds[p]),
            0, 0
        };

        cl_uint ndx = 0;
        for (auto it = devicesPerPlatform.begin(); it != devicesPerPlatform.end() - (platformIdCount - p); ++it) {
            ndx += *it;
        }

        context.push_back(clCreateContext(contextProperties, devicesPerPlatform[p],
                &deviceIds[ndx], NULL, NULL, &error));
    }

    // work_size defines the global work group size, which is defined by the size of the problem space
    work_size[0] = frWidth;
    work_size[1] = frHeight;
    work_size[2] = 1;

    // Save this for later
    std::fill(zero_buf.begin(), zero_buf.end(), 0);

    readyRead = BuildAndSetup();

    return readyRead;
}

bool StdDevFilter::BuildAndSetup()
{
    cl_int error = CL_SUCCESS;
    QString build_options("-DGPU_FRAME_BUFFER_SIZE=");
    build_options.append(QString::number(GPU_FRAME_BUFFER_SIZE));
    build_options.append(" -DNUMBER_OF_BINS="); // The space at the beginning of this string is important!
    build_options.append(QString::number(NUMBER_OF_BINS));

    program = CreateProgram(LoadKernel(":kernel/stddev.cl"), context[platform_num]);
    error = clBuildProgram(program, 1, &(deviceIds[device_num]),
                  build_options.toStdString().data(), 0, NULL);
    if (error != CL_SUCCESS) {
        cl_int errcode;
        size_t build_log_len;
        errcode = clGetProgramBuildInfo(program, deviceIds[device_num],
                CL_PROGRAM_BUILD_LOG, 0, NULL, &build_log_len);
        if (errcode) {
            qDebug("clGetProgramBuildInfo failed at line %d", __LINE__);
            return false;
        }

        std::vector<char> buff_erro(build_log_len);

        errcode = clGetProgramBuildInfo(program, deviceIds[device_num],
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

    devInputBuffer = clCreateBuffer(context[platform_num], CL_MEM_READ_ONLY,
            frWidth * frHeight * sizeof(cl_ushort) * GPU_FRAME_BUFFER_SIZE, NULL, &error);
    CheckError(error, __LINE__);

    devOutputBuffer = clCreateBuffer(context[platform_num], CL_MEM_WRITE_ONLY,
            frWidth * frHeight * sizeof(cl_float), NULL, &error);
    CheckError(error, __LINE__);

    hist_bins = clCreateBuffer(context[platform_num], CL_MEM_READ_ONLY,
             NUMBER_OF_BINS * sizeof(cl_float), NULL, &error);
     CheckError(error, __LINE__);

     devOutputHist = clCreateBuffer(context[platform_num], CL_MEM_WRITE_ONLY,
             NUMBER_OF_BINS * sizeof(cl_uint), NULL, &error);
     CheckError(error, __LINE__);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &devInputBuffer);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &devOutputBuffer);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &hist_bins);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), &devOutputHist);
    clSetKernelArg(kernel, 4, sizeof(cl_uint), &frWidth);
    clSetKernelArg(kernel, 5, sizeof(cl_uint), &frHeight);
    clSetKernelArg(kernel, 6, sizeof(cl_int), &gpu_buffer_head);
    clSetKernelArg(kernel, 7, sizeof(cl_uint), &N);

    commandQueue = clCreateCommandQueue(context[platform_num], deviceIds[device_num], 0, &error);
    CheckError(error, __LINE__);

    CheckError(clEnqueueWriteBuffer(commandQueue, hist_bins, CL_TRUE, 0, NUMBER_OF_BINS * sizeof(cl_float),
            getHistBinValues().data(), 0, NULL, NULL), __LINE__);
    CheckError(error, __LINE__);

    return true;
}

bool StdDevFilter::isReadyRead()
{
    return readyRead;
}

bool StdDevFilter::isReadyDisplay()
{
    return currentN == N;
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

cl_uint StdDevFilter::getPlatformNum(cl_uint dev_num)
{
    cl_uint plat_num = 0;
    cl_int devcounter = (cl_int)dev_num;
    for (auto &dpp : devicesPerPlatform) {
        devcounter -= dpp;
        if (devcounter > 0) {
            plat_num++;
        }
    }

    return plat_num;
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
    cl_event frame_written, hist_written, kernel_complete, frame_read, hist_read;
    if (new_N != N) {
        N = new_N;
        currentN = 0;
    }

    size_t devMemOffset = gpu_buffer_head * frWidth * frHeight * sizeof(cl_ushort);
    CheckError(clEnqueueWriteBuffer(commandQueue, devInputBuffer, CL_FALSE, devMemOffset, frWidth * frHeight * sizeof(cl_ushort),
                         new_frame->raw_data, 0, NULL, &frame_written), __LINE__);
    CheckError(clEnqueueWriteBuffer(commandQueue, devOutputHist, CL_FALSE, 0, NUMBER_OF_BINS * sizeof(cl_uint),
                zero_buf.data(), 0, NULL, &hist_written), __LINE__);

    const cl_event kernel_wait_list[2] = { frame_written, hist_written };

    // Now using a null pointer for the local work size, which can be determined automatically.
    CheckError(clEnqueueNDRangeKernel(commandQueue, kernel, 3,
                                      offset, work_size, NULL,
                                      2, kernel_wait_list, &kernel_complete), __LINE__);
    CheckError(clEnqueueReadBuffer(commandQueue, devOutputBuffer, CL_FALSE, 0, frWidth * frHeight * sizeof(cl_float),
                                                    new_frame->sdv_data, 1, &kernel_complete, &frame_read), __LINE__);
    CheckError(clEnqueueReadBuffer(commandQueue, devOutputHist, CL_FALSE, 0, NUMBER_OF_BINS * sizeof(cl_uint),
                                   new_frame->hist_data, 1, &kernel_complete, &hist_read), __LINE__);

    end_wait_list[0] = frame_read;
    end_wait_list[1] = hist_read;

    if (++gpu_buffer_head == GPU_FRAME_BUFFER_SIZE) {
        gpu_buffer_head = 0;
    }
    if (currentN < N) {
        currentN++;
    }
    count++;
    // wait for frame write completion
    CheckError(clWaitForEvents(2, end_wait_list), __LINE__);
}

QStringList StdDevFilter::getDeviceList()
{
    QStringList deviceNames;
    for (auto &device : deviceIds) {
        deviceNames << QString(GetDeviceName(device).data());
    }

    return deviceNames;
}

void StdDevFilter::change_device(QString dev_name)
{
    if (!QString::compare(QString(GetDeviceName(deviceIds[device_num]).data()), dev_name, Qt::CaseInsensitive)
            && !dev_name.isEmpty()) {
        for (size_t d = 0; d < deviceIds.size(); d++) {
            if (dev_name == GetDeviceName(deviceIds[d]).data()) {
                device_num = d;
            }
        }

        // wait for the last two asynch. events in the frame to end before running
        CheckError(clWaitForEvents(2, end_wait_list), __LINE__);
        readyRead = false; // stop execution of kernel
        currentN = 0;      // restart wait counter



        platform_num = getPlatformNum(device_num);

        // Release objects from old device and re-do JIT build process
        clReleaseMemObject(devInputBuffer);
        clReleaseMemObject(devOutputBuffer);
        clReleaseMemObject(hist_bins);
        clReleaseMemObject(devOutputHist);
        clReleaseCommandQueue(commandQueue);
        clReleaseKernel(kernel);
        clReleaseProgram(program);

        readyRead = BuildAndSetup();
    }
}
