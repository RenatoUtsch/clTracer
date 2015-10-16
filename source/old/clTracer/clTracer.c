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

/*
 * Implementation of the tracer.h functions using OpenCL.
 * This is an OpenCL RayTracer.
 */

#include "../tracer.h"
#include "../error.h"
#include "OpenCL.h"
#include "clUtils.h"

#ifndef CL_SOURCE_DIR // To be set by the compiler.
#define CL_SOURCE_DIR
#endif

// Where the tracer.cl file is.
#define TRACER_CLSOURCE_FILENAME CL_SOURCE_DIR "tracer.cl"

/// Tracer struct that holds all of OpenCL kernels, programs and memory objects.
struct Tracer {
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;

    cl_kernel sampleKernel;  /// Entry point of the raytracer.

    cl_mem imageBuffer;     /// Output image buffer.
};

Tracer *createTracer(size_t width, size_t height) {
    int err;

    Tracer *tracer = malloc(sizeof(*tracer));
    stop_if(!tracer, "failed to create OpenCL tracer.");

    err = clGetPlatformIDs(1, &tracer->platform, NULL);
    stop_if(err < 0, "failed to find an OpenCL platform.");

    err = clGetDeviceIDs(tracer->platform, CL_DEVICE_TYPE_GPU, 1,
            &tracer->device, NULL);
    stop_if(err < 0, "failed to find a GPU.");

    tracer->context = clCreateContext(NULL, 1, &tracer->device, NULL, NULL,
            &err);
    stop_if(err < 0, "failed to create an OpenCL context.");

    tracer->queue = clCreateCommandQueue(tracer->context, tracer->device, 0,
            &err);
    stop_if(err < 0, "failed to create an OpenCL command queue.");

    long sourceSize;
    err = cluLoadSource(TRACER_CLSOURCE_FILENAME, 0, NULL, &sourceSize);
    stop_if(err < 0, "failed to find the the OpenCL kernel source file:\n%s\n",
            TRACER_CLSOURCE_FILENAME);

    char *source = malloc(sourceSize);
    stop_if(!source, "failed to allocate the OpenCL kernel source buffer.");

    err = cluLoadSource(TRACER_CLSOURCE_FILENAME, sourceSize, source, NULL);
    stop_if(err < 0, "failed to load the OpenCL kernel source.");

    tracer->program = cluBuildProgram(tracer->context, tracer->device, source,
            sourceSize, &err);
    stop_if(err < 0, "failed to compile the OpenCL kernel.");

    tracer->sampleKernel = clCreateKernel(tracer->program, "sample", &err);
    stop_if(err < 0, "failed to create the sample kernel. Error %d.", err);

    cl_image_format rgba_format;
    rgba_format.image_channel_order = CL_RGBA;
    rgba_format.image_channel_data_type = CL_UNSIGNED_INT8;

    tracer->imageBuffer = clCreateImage2D(tracer->context, CL_MEM_WRITE_ONLY,
            &rgba_format, width, height, 0, NULL, &err);
    stop_if(err < 0, "failed to create the trace kernel output image. Error %d.", err);

    free(source);
    return tracer;
}

void destroyTracer(Tracer *tracer) {
    if(!tracer) return;

    clReleaseMemObject(tracer->imageBuffer);
    tracer->imageBuffer = NULL;

    clReleaseKernel(tracer->sampleKernel);
    tracer->sampleKernel = NULL;

    clReleaseCommandQueue(tracer->queue);

    clReleaseProgram(tracer->program);
    tracer->program = NULL;

    clReleaseContext(tracer->context);
    tracer->context = NULL;

    clReleaseDevice(tracer->device);
    tracer->device = NULL;

    free(tracer);
}

unsigned char *trace(Tracer *tracer, Scene *scene) {
    return NULL;
}

