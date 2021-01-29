#include <stdio.h>

int num_devices = 0;


/******************************************************************************
*
* Auxiliary routines
*
******************************************************************************/


#define error_check(error) do                 \
{                                             \
    cuda_assert((error), __FILE__, __LINE__); \
} while (0);

void cuda_assert(cudaError_t code, const char *file, int line)
{
    if (code != cudaSuccess) {
        fprintf(stderr,
            "[ERROR] Error code: %d Message: %s %s %d\n",
            code, cudaGetErrorString(code), file, line);
        exit(code);
    }
}

void mem_ustring(char *s, long bytes)
{
    double usize = 0;
    if (bytes > 1E+9)
    {
        usize = bytes / 1E+9;
        sprintf(s, "%.2lf GB", usize);
    }
    else if (bytes > 1E+6)
    {
        usize = bytes / 1E+6;
        sprintf(s, "%.2lf MB", usize);
    }
    else if (bytes > 1E+3)
    {
        usize = bytes / 1E+3;
        sprintf(s, "%.2lf KB", usize);
    }
    else
    {
        usize = bytes;
        sprintf(s, "%lf Bytes", usize);
    }
}

void print_device_info(void)
{
    for (int n = 0; n < num_devices; n++)
    {
        char ustring[64];

        cudaDeviceProp device_prop;
        cudaGetDeviceProperties(&device_prop, n);

        size_t free_mem;;

        cudaMemGetInfo(&free_mem, NULL);

        int device_threads = device_prop.multiProcessorCount * device_prop.maxThreadsPerMultiProcessor;

        printf("Device %d: %s\n", n, device_prop.name);
        printf("    Compute capability:      %d.%d\n", device_prop.major, device_prop.minor);
        printf("    Total number of threads: %d\n", device_threads);
        mem_ustring(ustring, device_prop.totalGlobalMem);
        printf("    Global memory size:      %s\n", ustring);
        mem_ustring(ustring, device_prop.sharedMemPerBlock);
        printf("    Shared memory size:      %s\n", ustring);
        mem_ustring(ustring, device_prop.totalConstMem);
        printf("    Constant memory size:    %s\n", ustring);
        mem_ustring(ustring, free_mem);
        printf("    Total free memory:       %s\n", ustring);
        printf("    Warp size:               %d\n", device_prop.warpSize);
    }
    printf("\n");
}


/******************************************************************************
*
* CUDA kernels
*
******************************************************************************/


__global__
void char_add(char *a, char *b)
{
    a[threadIdx.x] = b[threadIdx.x];
}


/******************************************************************************
*
* Device tests
*
******************************************************************************/


void run_device_test(void)
{
    for (int n = 0; n < num_devices; n++)
    {
        cudaSetDevice(n);

        int string_size;

        char string[256] = "Hello world!";

        string_size = strlen(string) + 1;

        /* Allocate device */
        char *d_a, *d_b;
        cudaMalloc((void **)&d_a, string_size * sizeof(char));
        cudaMalloc((void **)&d_b, string_size * sizeof(char));

        /* Allocate host */
        char *a = (char *)calloc(string_size, sizeof(char));
        char *b = (char *)calloc(string_size, sizeof(char));

        strcpy(b, string);

        dim3 dim_block(string_size, 1);
        dim3 dim_grid(1);

        cudaMemcpy((void *)d_b, (const void *)b, string_size * sizeof(char), cudaMemcpyHostToDevice);

        char_add<<<dim_grid, dim_block>>>(d_a, d_b);
        error_check(cudaPeekAtLastError());
        error_check(cudaDeviceSynchronize());

        cudaMemcpy((void *)a, (void *)d_a, string_size, cudaMemcpyDeviceToHost);

        cudaDeviceProp device_prop;
        cudaGetDeviceProperties(&device_prop, n);

        if (strcmp(a, b) == 0)
            printf("[PASSED TEST] Device %d: %s\n", n, device_prop.name);
        else
            printf("[FAILED TEST] Device %d: %s\n", n, device_prop.name);

        cudaFree(d_a);
        cudaFree(d_b);
        free(a);
        free(b);
    }
    printf("\n");
}

int main(void)
{
    cudaGetDeviceCount(&num_devices);
    if (num_devices > 0)
    {
        print_device_info();
        run_device_test();
    }
    else
    {
        printf("[ERROR] No CUDA devices found!\n");
    }

    return EXIT_SUCCESS;
}
