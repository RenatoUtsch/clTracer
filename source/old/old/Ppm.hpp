/*
 * Author: Renato Utsch Gonçalves
 * Computer Science, UFMG
 * Computer Graphics
 * Practical exercise 3 - Ray Tracer
 *
 * Copyright (c) 2014 Renato Utsch <renatoutsch@gmail.com>
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

#ifndef PPM_HPP
#define PPM_HPP

#include "Color.hpp"
#include <string>
#include <vector>

/**
 * Represents the data of a P6 ppm file.
 **/
class Ppm {
public:
    // Array of colors.
    std::vector< std::vector<Color> > data;

    // Height of the array (number of lines).
    size_t height;

    // Width of the array (number of columns).
    size_t width;

    Ppm() : height(0), width(0) {

    }

    /// Returns true if it is loaded.
    inline bool loaded() {
        return height == 0;
    }

    /// Loads a ppm image from a file. Throws std::runtime_error on failure.
    void load(const std::string &file);
};

#endif // !PPM_HPP
