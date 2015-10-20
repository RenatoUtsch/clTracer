/*
 * Author: Renato Utsch Gonçalves
 * Computer Science, UFMG
 * Advanced Computer Graphics
 * Practical exercise 1 - Distributed Ray Tracer
 *
 * Copyright (c) 2015 Renato Utsch <renatoutsch@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "../Sampler.hpp"
#include "../error.hpp"
#include "../utils.hpp"
#include "CodeGenerator.hpp"
#include "OpenCL.h"
#include <fstream>

#define XSTR(s) #s
#define STR(s) XSTR(s)

#ifndef CL_SOURCE_DIR // To be set by the compiler.
#define CL_SOURCE_DIR
#endif

// Maximum recursion depth.
#define MAX_DEPTH 4

// Where the sampler.cl file is.
#define SAMPLER_CLSOURCE_PATH CL_SOURCE_DIR "sampler.cl"

#ifdef __APPLE__ // Apple has really buggy GPU drivers.
#   define SAMPLER_DEVICE_TYPE CL_DEVICE_TYPE_CPU
#else
#   define SAMPLER_DEVICE_TYPE CL_DEVICE_TYPE_GPU
#endif

struct Sampler::SamplerImpl {
    int width, height;
    float pixelWidth, pixelHeight;
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;

    cl_kernel sampleKernel; /// Raytracer entry point.

    cl_mem originBuffer;    /// Origin of the ray.
    cl_mem topLeftBuffer;   /// Top left pixel position.
    cl_mem upBuffer;        /// Up vector
    cl_mem rightBuffer;     /// Right vector
    cl_mem outputImage;     /// Output image.

    Time time;              /// Used for benchmarking.

    /// Sets up OpenCL with the given program source code.
    SamplerImpl(const char *source, long sourceSize, int _width, int _height,
            float _pixelWidth, float _pixelHeight);

    ~SamplerImpl();

    /// Maps the output image to memory and returns a PPMImage representing it.
    std::shared_ptr<PPMImage> genOutputImage();

    /// Updates the sample kernel arguments.
    void updateSampleArgs(Screen &screen);

    /// Runs the sample kernel.
    void runSample();
};

Sampler::SamplerImpl::SamplerImpl(const char *source, long sourceSize,
        int _width, int _height, float _pixelWidth, float _pixelHeight)
        : width(_width), height(_height)
        , pixelWidth(_pixelWidth), pixelHeight(_pixelHeight)
{
    int err;

    err = clGetPlatformIDs(1, &platform, NULL);
    stop_if(err != CL_SUCCESS, "failed to find an OpenCL platform. Error %d.", err);

    err = clGetDeviceIDs(platform, SAMPLER_DEVICE_TYPE, 1, &device, NULL);
    stop_if(err != CL_SUCCESS, "failed to find a device. Error %d.", err);

    context = clCreateContext(NULL, 1, &device, NULL, NULL,
            &err);
    stop_if(err != CL_SUCCESS, "failed to create an OpenCL context. Error %d.", err);

    queue = clCreateCommandQueue(context, device, 0,
            &err);
    stop_if(err != CL_SUCCESS, "failed to create an OpenCL command queue. Error %d", err);

    program = cluBuildProgram(context, device, source, sourceSize,
            "-Werror -cl-strict-aliasing -cl-mad-enable -cl-no-signed-zeros "
            "-cl-unsafe-math-optimizations "
            "-DMAX_DEPTH=" STR(MAX_DEPTH), &err);
    stop_if(err < 0, "failed to compile the OpenCL kernel.");

    sampleKernel = clCreateKernel(program, "sample", &err);
    stop_if(err != CL_SUCCESS, "failed to create the sample kernel. Error %d.", err);

    originBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, 4 * sizeof(float),
            NULL, &err);
    stop_if(err != CL_SUCCESS, "failed to create the sample kernel origin position. "
            "Error %d.", err);

    topLeftBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, 4 * sizeof(float),
            NULL, &err);
    stop_if(err != CL_SUCCESS, "failed to create the sample kernel top left position. "
            "Error %d.", err);

    upBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, 4 * sizeof(float),
            NULL, &err);
    stop_if(err != CL_SUCCESS, "failed to create the sample kernel up vector. "
            "Error %d.", err);

    rightBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, 4 * sizeof(float),
            NULL, &err);
    stop_if(err != CL_SUCCESS, "failed to create the sample kernel right vector. "
            "Error %d.", err);

    cl_image_format rgbaFormat;
    rgbaFormat.image_channel_order = CL_RGBA;
    rgbaFormat.image_channel_data_type = CL_UNORM_INT8;

    outputImage = clCreateImage2D(context, CL_MEM_WRITE_ONLY,
            &rgbaFormat, width, height, 0, NULL, &err);
    stop_if(err != CL_SUCCESS,
            "failed to create the sample kernel output image. Error %d.", err);
    stop_if(!outputImage, "SHIT");

    err = clSetKernelArg(sampleKernel, 0, sizeof(originBuffer), &originBuffer);
    stop_if(err < 0, "failed to set first kernel argument. Error %d.", err);

    err = clSetKernelArg(sampleKernel, 1, sizeof(topLeftBuffer), &topLeftBuffer);
    stop_if(err < 0, "failed to set second kernel argument. Error %d.", err);

    err = clSetKernelArg(sampleKernel, 2, sizeof(upBuffer), &upBuffer);
    stop_if(err < 0, "failed to set third kernel argument. Error %d.", err);

    err = clSetKernelArg(sampleKernel, 3, sizeof(rightBuffer), &rightBuffer);
    stop_if(err < 0, "failed to set fourth kernel argument. Error %d.", err);

    err = clSetKernelArg(sampleKernel, 4, sizeof(float), &pixelWidth);
    stop_if(err < 0, "failed to set fifth kernel argument. Error %d.", err);

    err = clSetKernelArg(sampleKernel, 5, sizeof(float), &pixelHeight);
    stop_if(err < 0, "failed to set sixth kernel argument. Error %d.", err);

    err = clSetKernelArg(sampleKernel, 6, sizeof(outputImage), &outputImage);
    stop_if(err != CL_SUCCESS, "failed to set seventh kernel argument. Error %d.",
            err);
}

Sampler::SamplerImpl::~SamplerImpl() {
    clReleaseMemObject(outputImage);
    clReleaseMemObject(rightBuffer);
    clReleaseMemObject(upBuffer);
    clReleaseMemObject(topLeftBuffer);
    clReleaseMemObject(originBuffer);
    clReleaseKernel(sampleKernel);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);
    clReleaseDevice(device);
}

Sampler::Sampler(const World &world, int width, int height, float pixelWidth,
        float pixelHeight) {
    int err;
    CodeGenerator generator;

    // Generate the source that represents the given world.
    auto genSource = generator.generateCode(world);

    // Append the generated source with the sampler.cl source.

    long samplerSourceSize;
    err = cluLoadSource(SAMPLER_CLSOURCE_PATH, 0, NULL, &samplerSourceSize);
    stop_if(err < 0, "failed to find the the OpenCL kernel source file:\n%s\n",
            SAMPLER_CLSOURCE_PATH);

    char *samplerSource = new char[samplerSourceSize];
    stop_if(!samplerSource,
            "failed to allocate the OpenCL kernel source buffer.");

    err = cluLoadSource(SAMPLER_CLSOURCE_PATH, samplerSourceSize, samplerSource,
            NULL);
    stop_if(err < 0, "failed to load the OpenCL kernel source.");

    auto source = genSource + samplerSource;

    delete [] samplerSource;

    // Save the source to a file.
    std::ofstream out("source.cl");
    stop_if(!out.is_open(), "Failed to open output kernel file.");
    out << source;
    out.close();

    // Set up OpenCL.
    _impl = new SamplerImpl(source.c_str(), source.size(), width, height,
            pixelWidth, pixelHeight);
}

Sampler::~Sampler() {
    delete _impl;
}

void Sampler::SamplerImpl::updateSampleArgs(Screen &screen) {
    int err;

    if(screen.linearizedCameraPosNeedsUpdate()) {
        const float *vector = screen.linearizedCameraPos();

        float *mapped = (float *) clEnqueueMapBuffer(queue, originBuffer,
                CL_TRUE, CL_MAP_WRITE, 0, screen.LinearizedVectorSize, 0, NULL,
                NULL, &err);
        stop_if(err != CL_SUCCESS, "failed to map first kernel argument.");

        memcpy(mapped, vector, screen.LinearizedVectorSize);

        clEnqueueUnmapMemObject(queue, originBuffer, mapped, 0, NULL, NULL);
    }

    if(screen.linearizedTopLeftPixelNeedsUpdate()) {
        const float *vector = screen.linearizedTopLeftPixel();

        float *mapped = (float *) clEnqueueMapBuffer(queue, topLeftBuffer,
                CL_TRUE, CL_MAP_WRITE, 0, screen.LinearizedVectorSize, 0, NULL,
                NULL, &err);
        stop_if(err != CL_SUCCESS, "failed to map second kernel argument.");

        memcpy(mapped, vector, screen.LinearizedVectorSize);

        clEnqueueUnmapMemObject(queue, topLeftBuffer, mapped, 0, NULL, NULL);
    }

    if(screen.linearizedUpVectorNeedsUpdate()) {
        const float *vector = screen.linearizedUpVector();

        float *mapped = (float *) clEnqueueMapBuffer(queue, upBuffer,
                CL_TRUE, CL_MAP_WRITE, 0, screen.LinearizedVectorSize, 0, NULL,
                NULL, &err);
        stop_if(err != CL_SUCCESS, "failed to map third kernel argument.");

        memcpy(mapped, vector, screen.LinearizedVectorSize);

        clEnqueueUnmapMemObject(queue, upBuffer, mapped, 0, NULL, NULL);
    }

    if(screen.linearizedRightVectorNeedsUpdate()) {
        const float *vector = screen.linearizedRightVector();

        float *mapped = (float *) clEnqueueMapBuffer(queue, rightBuffer,
                CL_TRUE, CL_MAP_WRITE, 0, screen.LinearizedVectorSize, 0, NULL,
                NULL, &err);
        stop_if(err != CL_SUCCESS, "failed to map fourth kernel argument.");

        memcpy(mapped, vector, screen.LinearizedVectorSize);

        clEnqueueUnmapMemObject(queue, rightBuffer, mapped, 0, NULL, NULL);
    }
}

void Sampler::SamplerImpl::runSample() {
    size_t work_size[2] = {(size_t) width, (size_t) height};
    size_t global_offset[2] = {0, 0};

    // Start benchmarking the execution.
    time = getTime();

    int err = clEnqueueNDRangeKernel(queue, sampleKernel, 2, global_offset,
            work_size, NULL, 0, NULL, NULL);
    stop_if(err != CL_SUCCESS, "failed to enqueue kernel execution. Error %d.", err);

    // Wait for everything to end.
    clFinish(queue);
    stop_if(err != CL_SUCCESS, "failed to wait for queue to finish. Error %d.", err);

    // Print time.
    time = getTime() - time;
    std::cout << "Kernel execution time: " << time << "ms\n"
        << "Generating output..." << std::endl;
}

std::shared_ptr<PPMImage> Sampler::SamplerImpl::genOutputImage() {
    int err;

    // Map the entire output image.
    size_t rowPitch = 0;
    size_t origin[3] = {0, 0, 0};
    size_t region[3] = {(size_t) width, (size_t) height, 1};
    uint8_t *output = (uint8_t *) clEnqueueMapImage(queue, outputImage,
            CL_TRUE, CL_MAP_READ, origin, region, &rowPitch, NULL, 0, NULL,
            NULL, &err);
    stop_if(err != CL_SUCCESS, "failed to map output kernel image. Error %d.", err);

    auto image = std::make_shared<PPMImage>(output, width, height);

    clEnqueueUnmapMemObject(queue, outputImage, output, 0, NULL, NULL);

    return image;
}

std::shared_ptr<PPMImage> Sampler::sample(Screen &screen) {
    _impl->updateSampleArgs(screen);
    _impl->runSample();
    return _impl->genOutputImage();
}
