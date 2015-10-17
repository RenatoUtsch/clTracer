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

#include "PPMImage.hpp"
#include "error.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>

PPMImage::PPMImage(uint8_t *aImage, int aWidth, int aHeight)
        : _height(aHeight), _width(aWidth) {

    _data.resize(_height);
    for(int i = 0; i < _height; ++i) {
        _data[i].resize(_width);

        for(int j = 0; j < _width; ++j) {
            size_t pos = 4 * (_width * i + j);
            _data[i][j].fromRGB(aImage[pos], aImage[pos + 1], aImage[pos + 2]);
        }
    }
}

PPMImage::PPMImage(const char *filename) {
    std::ifstream in(filename);
    stop_if(!in, "failed to open ppm file.");

    std::string line;
    std::stringstream ss;
    int maxColor;

    getline(in, line);
    stop_if(line != "P6", "invalid ppm file. Only P6 format supported.");

    do {
        getline(in, line);
    } while(line[0] == '#'); // Ignore comments.

    ss << line;
    getline(in, line);
    ss << " " << line;
    ss >> _width >> _height >> maxColor;
    stop_if(_width <= 0 || _height <= 0 || maxColor <= 0 || maxColor > 255,
            "invalid ppm sizes (valid sizes are width > 0, height > 0, "
            "0 < maxColor <= 255)");

    uint8_t r, g, b;
    _data.resize(_height);
    for(int i = 0; i < _height; ++i) {
        _data[i].resize(_width);
        for(int j = 0; j < _width; ++j) {
            in >> r >> g >> b;
            stop_if(!in.good(), "bad ppm file.");

            _data[i][j].r = (float) r / maxColor;
            _data[i][j].g = (float) g / maxColor;
            _data[i][j].b = (float) b / maxColor;
        }
    }
}

void PPMImage::writeTo(const char *filename) {
    std::ofstream out(filename);
    stop_if(!out.is_open(), "failed to open output file (%s).", filename);

    // PPM header.
    out << "P6\n";
    out << "# Distributed Raytracer by RenatoUtsch <renatoutsch@gmail.com>\n";
    out << _width << " " << _height << "\n";
    out << "255\n";

    uint8_t r, g, b;
    for(int i = 0; i < _height; ++i) {
        for(int j = 0; j < _width; ++j) {
            _data[i][j].toRGB(&r, &g, &b);
            out << r << g << b;
        }
    }
}
