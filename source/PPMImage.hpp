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

#ifndef PPMIMAGE_HPP
#define PPMIMAGE_HPP

#include "Color.hpp"

#include <cstdint>
#include <vector>
#include <string>

/**
 * This class represents a single PPM image.
 */
class PPMImage {
    /// Height of the matrix (number of lines).
    int _height;

    /// Width of the matrix (number of columns).
    int _width;

public:

    /// Matrix of colors.
    std::vector< std::vector<Color> > data;

    /**
     * Constructs the PPM image from the given input in 32bit RGBA format.
     * Please note that the 4th component (the alpha channel) will be ignored.
     */
    PPMImage(uint8_t *aImage, int aWidth, int aHeight);

    /**
     * Constructs the PPM image from the given PPM file.
     */
    PPMImage(const std::string &filename);

    /**
     * Writes the PPM image to the file with the given filename.
     */
    void writeTo(const std::string &filename);

    /**
     * Width of the image.
     */
    inline int width() const {
        return _width;
    }

    /**
     * Height of the image.
     */
    inline int height() const {
        return _height;
    }
};

#endif // !PPMIMAGE_HPP
