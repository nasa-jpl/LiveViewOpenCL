__kernel void
std_dev_filter_kernel(__global const ushort *d_ipic,
                      __global float *d_opic,
                      uint width, uint height,
                      int gpu_buffer_head, uint N)
{
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
}
