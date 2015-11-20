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

#ifndef SAMPLERIMPL_HPP
#define SAMPLERIMPL_HPP

#include "../Sampler.hpp"
#include "../utils.hpp"
#include "OpenCL.h"

struct Sampler::SamplerImpl {
    int width, height;
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
    SamplerImpl(const char *source, long sourceSize, int _width, int _height);

    ~SamplerImpl();
};

#endif // !SAMPLERIMPL_HPP
