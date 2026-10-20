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

	const int size = 10;
	int a[] = {0, 2, 5, 8, 9};
	int b[] = {1, 3, 4, 6, 7};
	int c[size];

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

	cl::Buffer bufferA(
		*clu_Context,
		CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		size/2 * sizeof(int),
		a	
	);

	cl::Buffer bufferB(
		*clu_Context,
		CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		size/2 * sizeof(int),
		b	
	);

	cl::Buffer bufferOut(
		*clu_Context,
		CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		size * sizeof(int),
		c	
	);

	// Doing all the swaps
	for (int i = 0; i < size; i++)
	{
		err = g_Kernel->setArg(0, bufferA);
		err = g_Kernel->setArg(1, bufferB);
		err = g_Kernel->setArg(2, size);
		err = g_Kernel->setArg(3, bufferOut);

		err = clu_Queue->enqueueNDRangeKernel(
			*g_Kernel,
			cl::NullRange,
			cl::NDRange(size),
			cl::NullRange
		);

		std::cout << "Kernel error code: " << err << std::endl;

		clu_Queue->finish();

	}

	err = clu_Queue->enqueueReadBuffer(
		bufferOut,
		true,
		0,
		size * sizeof(int),
		c	
	);

	for (int i = 0; i < size; i++) {
        std::cout << c[i] << " ";
    }
    std::cout << std::endl;

}
