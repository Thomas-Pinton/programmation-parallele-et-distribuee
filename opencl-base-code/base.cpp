#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip> 
#include <chrono>

#include "lodepng.h"

using namespace std;

// We provide a small library so we can easily setup OpenCL
#include "clutils.h"

// ----------------------------------------------------------

void decodeOneStep(const char* filename, unsigned int* width, unsigned int* height, std::vector<unsigned char>* image) {
  std::vector<unsigned char> png;

  // load the file
  unsigned error = lodepng::load_file(png, filename);
  
  // decode to 8-bit grayscale (LCT_GREY)
  if(!error) {
      error = lodepng::decode(*image, *width, *height, png, LCT_GREY, 8);
  }

  // if there's an error, display it
  if(error) {
      std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
  }

  // the pixels are now in the vector "image", 1 byte per pixel, ordered VVVVV...
  // V = Value (0-255). 
  // The vector size will be exactly (width * height) bytes.
}

void encodeOneStep(const char* filename, std::vector<unsigned char>& image, unsigned width, unsigned height) {
  std::cout << "encoding" << std::endl;
  //Encode the image
  unsigned error = lodepng::encode(filename, image, width, height, LCT_GREY, 8);

  //if there's an error, display it
  if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
}

int main(int argc, char **argv)
{
	int iterations = std::atoi(argv[1]);
    int isMax = std::atoi(argv[2]);

	unsigned int width, height;
	std::vector<unsigned char> image; // the raw pixels
	std::vector<unsigned char> imageOut; 

	decodeOneStep("einstein_512.png", &width, &height, &image);

	imageOut.resize(image.size() / 4);

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

	for (int i = 0; i < iterations; i++)
	{
		cl::Buffer inBuffer(
			*clu_Context,
			CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			image.size(),
			image.data()
		);

		cl::Buffer outBuffer(
			*clu_Context,
			CL_MEM_WRITE_ONLY,
			image.size() / 4,
			NULL
		);

		err = g_Kernel->setArg(0, inBuffer);
		err = g_Kernel->setArg(1, width);
		err = g_Kernel->setArg(2, isMax);
		err = g_Kernel->setArg(3, outBuffer);

		err = clu_Queue->enqueueNDRangeKernel(
			*g_Kernel,
			cl::NullRange,
			cl::NDRange(width / 2, height / 2),
			cl::NullRange
		);

		std::cout << "Kernel error code: " << err << std::endl;

		err = clu_Queue->enqueueReadBuffer(
			outBuffer,
			false,
			0,
			image.size() / 4 * sizeof(unsigned char),
			imageOut.data()	
		);

		clu_Queue->finish();

		std::string filename = "results/L_" + std::to_string(i) + ".png";
		encodeOneStep(filename.c_str(), imageOut, width / 2, height / 2);

		image = imageOut;
		width = width / 2;
		height = height / 2;
		imageOut.resize(image.size() / 4);
	}

}
