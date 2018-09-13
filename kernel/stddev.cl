#pragma OPENCL EXTENSION cl_khr_fp64: enable

__kernel void
std_dev_filter_kernel(__global const ushort *d_ipic,
                      __global float *d_opic,
                      __global float *histogram_bins,
                      __global uint *d_ohist,
                      uint width, uint height,
                      int gpu_buffer_head, uint N)
{
    __local uint block_histogram[NUMBER_OF_BINS];
    uint col = get_global_id(0);
    uint row = get_global_id(1);
    uint offset = col + row * width;
    uint fr_size = height * width;

    double sum = 0;
    double sq_sum = 0;
    double mean = 0;
    float std_dev;
    int value = 0;
    int c = 0;

    for (uint i = 0; i < N; ++i) {
        if ((gpu_buffer_head - (int)i) >= 0) {
            value = *(d_ipic + offset + (fr_size * (gpu_buffer_head - i)));
        } else {
            value = *(d_ipic + offset + (fr_size * (GPU_FRAME_BUFFER_SIZE - (i - gpu_buffer_head))));
        }
        sum += value;
        sq_sum += (double)value * (double)value;
    }

    mean = sum / (double)N;
    float disc = ((sq_sum - (double)(2 * mean * sum)) / (double)N) + mean * mean;
    std_dev = sqrt(disc);
    d_opic[offset] = std_dev;

    barrier(CLK_GLOBAL_MEM_FENCE);
    int groupArea = get_local_size(0) * get_local_size(1);
    for (int shm_offset = 0; shm_offset < NUMBER_OF_BINS; shm_offset+=groupArea) {
        if (shm_offset + get_local_id(1) * get_local_size(0) + get_local_id(0) < NUMBER_OF_BINS) {
            block_histogram[shm_offset + get_local_id(1) * get_local_size(0) + get_local_id(0)] = 0;
        }
    }

    while (std_dev > histogram_bins[c] && c < (NUMBER_OF_BINS - 1)) {
        c++;
    }

    barrier(CLK_LOCAL_MEM_FENCE);
    atomic_add(&block_histogram[c], 1);
    barrier(CLK_LOCAL_MEM_FENCE);

    if (get_local_id(0) == 0 && get_local_id(1) == 0) {
        for (c = 0; c < NUMBER_OF_BINS; c++) {
            atomic_add(&d_ohist[c], block_histogram[c]);
        }
    }

}
