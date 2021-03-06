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
#include "CodeGenerator.hpp"
#include "../error.hpp"

#define XSTR(s) #s
#define STR(s) XSTR(s)

#ifndef CL_SOURCE_DIR // To be set by the compiler.
#define CL_SOURCE_DIR
#endif

// Where the sampler.cl file is.
#define SAMPLER_CLSOURCE_PATH CL_SOURCE_DIR "sampler.cl"

// GPU drivers are simply horrible.
#define SAMPLER_DEVICE_TYPE CL_DEVICE_TYPE_CPU

Sampler::SamplerImpl::SamplerImpl(const World &world, const Screen &screen,
        const CmdArgs &args)
        : _width{screen.width()}, _height{screen.height()} {
    int err;

    cl_platform_id *platforms;
    unsigned numPlatforms;

    err = clGetPlatformIDs(0, NULL, &numPlatforms);
    stop_if(err < 0, "failed to find number of OpenCL platforms. Error %d.", err);

    platforms = new cl_platform_id[numPlatforms];

    err = clGetPlatformIDs(numPlatforms, platforms, NULL);
    stop_if(err < 0, "failed to find OpenCL platforms. Error %d.", err);


    for(size_t i = 0; i < numPlatforms; ++i) {
        err = clGetDeviceIDs(platforms[i], SAMPLER_DEVICE_TYPE, 1, &_device, NULL);
        if(err >= 0) {
            _platform = platforms[i];
            break;
        }
    }
    delete[] platforms;
    stop_if(err < 0, "failed to find a device. Error %d.", err);

    _context = clCreateContext(NULL, 1, &_device, NULL, NULL,
            &err);
    stop_if(err < 0, "failed to create an OpenCL context. Error %d.", err);

    _queue = clCreateCommandQueue(_context, _device, 0, &err);
    stop_if(err < 0, "failed to create an OpenCL command queue. Error %d", err);

    auto source = generateSource(world, screen, args);
    _program = cluBuildProgram(_context, _device, source.c_str(), source.size(),
            "-I " CL_SOURCE_DIR " "
            "-Werror -cl-mad-enable -cl-no-signed-zeros "
            "-cl-unsafe-math-optimizations -cl-fast-relaxed-math ",
            &err);
    stop_if(err < 0, "failed to compile the OpenCL kernel.");

    _sampleKernel = clCreateKernel(_program, "sample", &err);
    stop_if(err < 0, "failed to create the sample kernel. Error %d.", err);

    constructBuffers(screen);
}

Sampler::SamplerImpl::~SamplerImpl() {
    clReleaseMemObject(_outputImage);
    clReleaseMemObject(_rightBuffer);
    clReleaseMemObject(_upBuffer);
    clReleaseMemObject(_topLeftBuffer);
    clReleaseMemObject(_originBuffer);
    clReleaseKernel(_sampleKernel);
    clReleaseCommandQueue(_queue);
    clReleaseProgram(_program);
    clReleaseContext(_context);
    clReleaseDevice(_device);
}

std::string Sampler::SamplerImpl::generateSource(const World &world,
        const Screen &screen, const CmdArgs &args) {
    CodeGenerator generator;

    // Generate the source that represents the given world.
    auto genSource = generator.generateCode(world, screen, args);

#ifdef DEBUG
    // Save the source to a file.
    std::ofstream out("source.cl");
    stop_if(!out.is_open(), "Failed to open output kernel file.");
    out << genSource;
    out.close();
#endif

    return genSource;
}

void Sampler::SamplerImpl::constructBuffers(const Screen &screen) {
    int err;

    _originBuffer = clCreateBuffer(_context, CL_MEM_READ_ONLY, 4 * sizeof(float),
            NULL, &err);
    stop_if(err < 0, "failed to create the sample kernel origin position. "
            "Error %d.", err);

    _topLeftBuffer = clCreateBuffer(_context, CL_MEM_READ_ONLY, 4 * sizeof(float),
            NULL, &err);
    stop_if(err < 0, "failed to create the sample kernel top left position. "
            "Error %d.", err);

    _upBuffer = clCreateBuffer(_context, CL_MEM_READ_ONLY, 4 * sizeof(float),
            NULL, &err);
    stop_if(err < 0, "failed to create the sample kernel up vector. "
            "Error %d.", err);

    _rightBuffer = clCreateBuffer(_context, CL_MEM_READ_ONLY, 4 * sizeof(float),
            NULL, &err);
    stop_if(err < 0, "failed to create the sample kernel right vector. "
            "Error %d.", err);

    cl_image_format rgbaFormat;
    rgbaFormat.image_channel_order = CL_RGBA;
    rgbaFormat.image_channel_data_type = CL_UNORM_INT8;

    _outputImage = clCreateImage2D(_context, CL_MEM_WRITE_ONLY,
            &rgbaFormat, _width, _height, 0, NULL, &err);
    stop_if(err < 0,
            "failed to create the sample kernel output image. Error %d.", err);


    float *mapped;

    mapped = (float *) clEnqueueMapBuffer(_queue, _originBuffer, CL_TRUE,
            CL_MAP_WRITE, 0, screen.ArraySize, 0, NULL, NULL, &err);
    stop_if(err < 0, "failed to map first kernel argument.");

    memcpy(mapped, screen.cameraPos(), screen.ArraySize);
    clEnqueueUnmapMemObject(_queue, _originBuffer, mapped, 0, NULL, NULL);

    mapped = (float *) clEnqueueMapBuffer(_queue, _topLeftBuffer, CL_TRUE,
            CL_MAP_WRITE, 0, screen.ArraySize, 0, NULL, NULL, &err);
    stop_if(err < 0, "failed to map second kernel argument.");

    memcpy(mapped, screen.topLeftPixelPos(), screen.ArraySize);
    clEnqueueUnmapMemObject(_queue, _topLeftBuffer, mapped, 0, NULL, NULL);

    mapped = (float *) clEnqueueMapBuffer(_queue, _upBuffer, CL_TRUE,
            CL_MAP_WRITE, 0, screen.ArraySize, 0, NULL, NULL, &err);
    stop_if(err < 0, "failed to map third kernel argument.");

    memcpy(mapped, screen.upVector(), screen.ArraySize);
    clEnqueueUnmapMemObject(_queue, _upBuffer, mapped, 0, NULL, NULL);

    mapped = (float *) clEnqueueMapBuffer(_queue, _rightBuffer, CL_TRUE,
            CL_MAP_WRITE, 0, screen.ArraySize, 0, NULL, NULL, &err);
    stop_if(err < 0, "failed to map fourth kernel argument.");

    memcpy(mapped, screen.rightVector(), screen.ArraySize);
    clEnqueueUnmapMemObject(_queue, _rightBuffer, mapped, 0, NULL, NULL);


    err = clSetKernelArg(_sampleKernel, 0, sizeof(_originBuffer), &_originBuffer);
    stop_if(err < 0, "failed to set first kernel argument. Error %d.", err);

    err = clSetKernelArg(_sampleKernel, 1, sizeof(_topLeftBuffer), &_topLeftBuffer);
    stop_if(err < 0, "failed to set second kernel argument. Error %d.", err);

    err = clSetKernelArg(_sampleKernel, 2, sizeof(_upBuffer), &_upBuffer);
    stop_if(err < 0, "failed to set third kernel argument. Error %d.", err);

    err = clSetKernelArg(_sampleKernel, 3, sizeof(_rightBuffer), &_rightBuffer);
    stop_if(err < 0, "failed to set fourth kernel argument. Error %d.", err);

    uint32_t seed[2] = {42, 84};
    err = clSetKernelArg(_sampleKernel, 4, 2 * sizeof(uint32_t), &seed);
    stop_if(err < 0, "failed to set fifth kernel argument. Error %d.", err);

    err = clSetKernelArg(_sampleKernel, 5, sizeof(_outputImage), &_outputImage);
    stop_if(err < 0, "failed to set sixth kernel argument. Error %d.", err);
}

std::unique_ptr<PPMImage> Sampler::SamplerImpl::sample() {
    int err;

    // Start benchmarking the execution.
    auto time = getTime();

    size_t workSize[2] = {(size_t) _width, (size_t) _height};
    size_t globalOffset[2] = {0, 0};
    err = clEnqueueNDRangeKernel(_queue, _sampleKernel, 2,
            globalOffset, workSize, NULL, 0, NULL, NULL);
    stop_if(err < 0, "failed to enqueue kernel execution. Error %d.", err);

    // Wait for everything to end.
    clFinish(_queue);
    stop_if(err < 0, "failed to wait for queue to finish. Error %d.", err);

    // Print time.
    std::cout << "Kernel execution time: " << getTime() - time << "ms\n"
        << "Generating output..." << std::endl;

    // Map the entire output image.
    size_t rowPitch = 0;
    size_t origin[3] = {0, 0, 0};
    size_t region[3] = {(size_t) _width, (size_t) _height, 1};
    uint8_t *output = (uint8_t *) clEnqueueMapImage(_queue, _outputImage,
            CL_TRUE, CL_MAP_READ, origin, region, &rowPitch, NULL, 0, NULL,
            NULL, &err);
    stop_if(err < 0, "failed to map output kernel image. Error %d.", err);

    auto image = std::make_unique<PPMImage>(output, _width, _height);

    clEnqueueUnmapMemObject(_queue, _outputImage, output, 0, NULL, NULL);

    return image;
}
