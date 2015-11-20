/*
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

#include "SamplerImpl.hpp"
#include "../error.hpp"

#define XSTR(s) #s
#define STR(s) XSTR(s)

#ifndef CL_SOURCE_DIR // To be set by the compiler.
#define CL_SOURCE_DIR
#endif

#ifdef __APPLE__ // Apple has really buggy GPU drivers.
#   define SAMPLER_DEVICE_TYPE CL_DEVICE_TYPE_CPU
#else
#   define SAMPLER_DEVICE_TYPE CL_DEVICE_TYPE_GPU
#endif

// Maximum recursion depth.
#define MAX_DEPTH 4

Sampler::SamplerImpl::SamplerImpl(const char *source, long sourceSize,
        int _width, int _height)
        : width(_width), height(_height) {
    int err;

    err = clGetPlatformIDs(1, &platform, NULL);
    stop_if(err < 0, "failed to find an OpenCL platform. Error %d.", err);

    err = clGetDeviceIDs(platform, SAMPLER_DEVICE_TYPE, 1, &device, NULL);
    stop_if(err < 0, "failed to find a device. Error %d.", err);

    context = clCreateContext(NULL, 1, &device, NULL, NULL,
            &err);
    stop_if(err < 0, "failed to create an OpenCL context. Error %d.", err);

    queue = clCreateCommandQueue(context, device, 0,
            &err);
    stop_if(err < 0, "failed to create an OpenCL command queue. Error %d", err);

    program = cluBuildProgram(context, device, source, sourceSize,
            "-I " CL_SOURCE_DIR " "
            "-Werror -cl-strict-aliasing -cl-mad-enable -cl-no-signed-zeros "
            "-cl-unsafe-math-optimizations -cl-finite-math-only -cl-fast-relaxed-math "
            "-DMAX_DEPTH=" STR(MAX_DEPTH), &err);
    stop_if(err < 0, "failed to compile the OpenCL kernel.");

    sampleKernel = clCreateKernel(program, "sample", &err);
    stop_if(err < 0, "failed to create the sample kernel. Error %d.", err);

    originBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, 4 * sizeof(float),
            NULL, &err);
    stop_if(err < 0, "failed to create the sample kernel origin position. "
            "Error %d.", err);

    topLeftBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, 4 * sizeof(float),
            NULL, &err);
    stop_if(err < 0, "failed to create the sample kernel top left position. "
            "Error %d.", err);

    upBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, 4 * sizeof(float),
            NULL, &err);
    stop_if(err < 0, "failed to create the sample kernel up vector. "
            "Error %d.", err);

    rightBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, 4 * sizeof(float),
            NULL, &err);
    stop_if(err < 0, "failed to create the sample kernel right vector. "
            "Error %d.", err);

    cl_image_format rgbaFormat;
    rgbaFormat.image_channel_order = CL_RGBA;
    rgbaFormat.image_channel_data_type = CL_UNORM_INT8;

    outputImage = clCreateImage2D(context, CL_MEM_WRITE_ONLY,
            &rgbaFormat, width, height, 0, NULL, &err);
    stop_if(err < 0,
            "failed to create the sample kernel output image. Error %d.", err);

    err = clSetKernelArg(sampleKernel, 0, sizeof(originBuffer), &originBuffer);
    stop_if(err < 0, "failed to set first kernel argument. Error %d.", err);

    err = clSetKernelArg(sampleKernel, 1, sizeof(topLeftBuffer), &topLeftBuffer);
    stop_if(err < 0, "failed to set second kernel argument. Error %d.", err);

    err = clSetKernelArg(sampleKernel, 2, sizeof(upBuffer), &upBuffer);
    stop_if(err < 0, "failed to set third kernel argument. Error %d.", err);

    err = clSetKernelArg(sampleKernel, 3, sizeof(rightBuffer), &rightBuffer);
    stop_if(err < 0, "failed to set fourth kernel argument. Error %d.", err);
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
