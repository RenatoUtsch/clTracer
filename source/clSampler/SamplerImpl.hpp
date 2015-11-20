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

class Sampler::SamplerImpl {
    int _width, _height;
    cl_platform_id _platform;
    cl_device_id _device;
    cl_context _context;
    cl_command_queue _queue;
    cl_program _program;

    cl_kernel _sampleKernel; /// Path Tracer entry point.

    cl_mem _originBuffer;    /// Origin of the ray.
    cl_mem _topLeftBuffer;   /// Top left pixel position.
    cl_mem _upBuffer;        /// Up vector
    cl_mem _rightBuffer;     /// Right vector
    cl_mem _outputImage;     /// Output image.

    std::string generateSource(const World &world, const Screen &screen,
            const CmdArgs &args);
    void constructBuffers(const Screen &screen);

public:
    SamplerImpl() = delete;

    /**
     * Creates the SamplerImpl object.
     * Look at the Sampler() constructor for more information.
     */
    SamplerImpl(const World &world, const Screen &screen, const CmdArgs &args);

    ~SamplerImpl();

    /// Samples all pixels and returns the image.
    std::unique_ptr<PPMImage> sample();
};

#endif // !SAMPLERIMPL_HPP
