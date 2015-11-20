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

#ifndef SAMPLER_HPP
#define SAMPLER_HPP

#include "World.hpp"
#include "Screen.hpp"
#include "CmdArgs.hpp"
#include "PPMImage.hpp"

#include <cstdint>
#include <memory>

/**
 * Class that actually samples each pixel by tracing the ray from the camera
 * position to the pixel and calculates the generated image.
 */
class Sampler {
    /// Sampler implementation.
    class SamplerImpl;

    /// Pointer to the sampler implementation.
    std::unique_ptr<SamplerImpl> _impl;

public:
    /**
     * Creates a sampler object.
     * @param world All the constant information about the world. This will be
     * constant through all the execution of the sampler.
     * @param screen Screen properties.
     * @param args Program command line arguments.
     */
    Sampler(const World &world, const Screen &screen, const CmdArgs &args);

    ~Sampler();

    /**
     * Samples all the pixels from the screen used when creating this class.
     * Returns the sampled image.
     * This is the actual path tracing call.
     */
    std::unique_ptr<PPMImage> sample();
};

#endif // !SAMPLER_HPP
