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
#include "CodeGenerator.hpp"
#include "OpenCL.h"

#ifndef CL_SOURCE_DIR // To be set by the compiler.
#define CL_SOURCE_DIR
#endif

// Where the sampler.cl file is.
#define SAMPLER_CLSOURCE_PATH CL_SOURCE_DIR "sampler.cl"

struct Sampler::SamplerImpl {
    int width, height;
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;

    cl_kernel sampleKernel; /// Raytracer entry point.

    cl_mem originBuffer;    /// Origin of the ray.
    cl_mem directionBuffer; /// Direction of the rays.
    cl_mem outputImage;     /// Output image.

    /// Sets up OpenCL with the given program source code.
    SamplerImpl(const char *source, long sourceSize, int _width, int _height);

    ~SamplerImpl();

    /// Maps the output image to memory and returns a PPMImage representing it.
    std::shared_ptr<PPMImage> genOutputImage();

    /// Updates the sample kernel arguments.
    void updateSampleArgs(const Screen &screen);

    /// Runs the sample kernel.
    void runSample();
};

Sampler::SamplerImpl::SamplerImpl(const char *source, long sourceSize,
        int _width, int _height) : width(_width), height(_height) {
    int err;

    err = clGetPlatformIDs(1, &platform, NULL);
    stop_if(err != CL_SUCCESS, "failed to find an OpenCL platform. Error %d.", err);

    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1,
            &device, NULL);
    stop_if(err != CL_SUCCESS, "failed to find a GPU. Error %d.", err);

    context = clCreateContext(NULL, 1, &device, NULL, NULL,
            &err);
    stop_if(err != CL_SUCCESS, "failed to create an OpenCL context. Error %d.", err);

    queue = clCreateCommandQueue(context, device, 0,
            &err);
    stop_if(err != CL_SUCCESS, "failed to create an OpenCL command queue. Error %d", err);

    program = cluBuildProgram(context, device, source, sourceSize, &err);
    stop_if(err < 0, "failed to compile the OpenCL kernel.");

    sampleKernel = clCreateKernel(program, "sample", &err);
    stop_if(err != CL_SUCCESS, "failed to create the sample kernel. Error %d.", err);

    originBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, 4 * sizeof(float),
            NULL, &err);
    stop_if(err != CL_SUCCESS, "failed to create the sample kernel origin position. "
            "Error %d.", err);

    directionBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY,
            width * height * 4 * sizeof(float), NULL, &err);
    stop_if(err != CL_SUCCESS, "failed to create the sample kernel input directions. "
            "Error %d.", err);

    cl_image_format rgba_format;
    rgba_format.image_channel_order = CL_RGBA;
    rgba_format.image_channel_data_type = CL_UNSIGNED_INT8;

    outputImage = clCreateImage2D(context, CL_MEM_WRITE_ONLY,
            &rgba_format, width, height, 0, NULL, &err);
    stop_if(err != CL_SUCCESS,
            "failed to create the sample kernel output image. Error %d.", err);
    stop_if(!outputImage, "SHIT");

    /*
    err = clSetKernelArg(sampleKernel, 0, sizeof(originBuffer), &originBuffer);
    stop_if(err < 0, "failed to set first kernel argument.");

    err = clSetKernelArg(sampleKernel, 1, sizeof(directionBuffer),
            &directionBuffer);
    stop_if(err < 0, "failed to set second kernel argument.");
    */

    err = clSetKernelArg(sampleKernel, 0, sizeof(outputImage), &outputImage);
    stop_if(err != CL_SUCCESS, "failed to set third kernel argument. Error %d.", err);
}

Sampler::SamplerImpl::~SamplerImpl() {
    clReleaseMemObject(outputImage);
    clReleaseMemObject(directionBuffer);
    clReleaseMemObject(originBuffer);
    clReleaseKernel(sampleKernel);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);
    clReleaseDevice(device);
}

Sampler::Sampler(const World &world, int width, int height) {
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
    std::cerr << source;

    // Set up OpenCL.
    _impl = new SamplerImpl(source.c_str(), source.size(), width, height);
}

Sampler::~Sampler() {
    delete _impl;
}

void Sampler::SamplerImpl::updateSampleArgs(const Screen &screen) {

}

void Sampler::SamplerImpl::runSample() {
    size_t work_size[2] = {(size_t) height, (size_t) width};
    size_t global_offset[2] = {0, 0};

    int err = clEnqueueNDRangeKernel(queue, sampleKernel, 2, global_offset,
            work_size, NULL, 0, NULL, NULL);
    stop_if(err != CL_SUCCESS, "failed to enqueue kernel execution. Error %d.", err);
}

std::shared_ptr<PPMImage> Sampler::SamplerImpl::genOutputImage() {
    int err;

    // Wait for everything to end.
    clFinish(queue);

    // Map the entire output image.
    size_t row_pitch;
    size_t origin[3] = {0, 0, 0};
    size_t region[3] = {(size_t) width, (size_t) height, 1};
    uint8_t *output = (uint8_t *) clEnqueueMapImage(queue, outputImage,
            CL_TRUE, CL_MAP_READ, origin, region, &row_pitch, NULL, 0, NULL,
            NULL, &err);
    stop_if(err != CL_SUCCESS, "failed to map output kernel image. Error %d.", err);

    for(int i = 0; i < 5; ++i)
        printf("%u %u %u %u\n", output[i*4], output[i*4 + 1], output[i*4 + 2],
                output[i*4 + 3]);

    auto image = std::make_shared<PPMImage>((uint8_t *)output, width, height);

    clEnqueueUnmapMemObject(queue, outputImage, output, 0, NULL, NULL);

    return image;
}

std::shared_ptr<PPMImage> Sampler::sample(const Screen &screen) {
    _impl->updateSampleArgs(screen);
    _impl->runSample();
    return _impl->genOutputImage();
}
