#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip> 
#include <chrono>

using namespace std;

// We provide a small library so we can easily setup OpenCL
#include "clutils.h"

// ----------------------------------------------------------

int main(int argc, char **argv)
{
    // int iterations = std::atoi(argv[1]);
    // int isMax = std::atoi(argv[2]);

    int size = 8;
    int numbers[] = {0, 4, 9, 3, 2, 4, 2, 1};

    const char *clu_File = SRC_PATH "base.cl";  // path to file containing OpenCL kernel(s) code

    // Initialize OpenCL
    cluInit();

    // After this call you have access to
    // clu_Context;      <= OpenCL context (pointer)
    // clu_Devices;      <= OpenCL device list (vector)
    // clu_Queue;        <= OpenCL queue (pointer)

    // Load Program
    cl::Program *program = cluLoadProgram(clu_File);

    cl_int err;

    cl::Kernel *g_Kernel = new cl::Kernel(*program, "mainKernel", &err);

    // Buffers for one swap
    cl::Buffer inBuffer(
        *clu_Context,
        CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
        size * sizeof(int),
        numbers
    );

    int isEven = 0;

    // Doing all the swaps
    for (int i = 0; i < size; i++)
    {
        err = g_Kernel->setArg(0, inBuffer);
        err = g_Kernel->setArg(1, size);
        err = g_Kernel->setArg(2, isEven);

        err = clu_Queue->enqueueNDRangeKernel(
            *g_Kernel,
            cl::NullRange,
            cl::NDRange(size / 2),
            cl::NullRange
        );

        std::cout << "Kernel error code: " << err << std::endl;

        clu_Queue->finish();

        isEven = 1 - isEven;
    }

    err = clu_Queue->enqueueReadBuffer(
        inBuffer,
        true,
        0,
        size * sizeof(int),
        numbers
    );

	for (int i = 0; i < size; i++) {
        std::cout << numbers[i] << " ";
    }
    std::cout << std::endl;

}