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

#include "Ppm.hpp"
#include "error.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

void Ppm::load(const std::string &file) {
    std::ifstream in(file.c_str());
    if(!in)
        throw std::runtime_error("Failed to open ppm file");

    std::string line;
    std::stringstream ss;
    int maxColor;

    getline(in, line);
    if(line != "P6")
        throw std::runtime_error("Invalid ppm file");

    do {
        getline(in, line);
    } while(line[0] == '#');

    ss << line;
    getline(in, line);
    ss << " " << line;
    ss >> width >> height >> maxColor;

    if(width <= 0 || height <= 0 || maxColor <= 0)
        throw std::runtime_error("Ppm sizes are invalid");
    if(maxColor > 255)
        throw std::runtime_error("Invalid maxColor (max is 255)");

    data.resize(height);

    // Store all the bytes.
    unsigned char r, g, b;
    Color c;
    for(size_t i = 0; i < height; ++i) {
        for(size_t j = 0; j < width; ++j) {
            in >> r >> g >> b;
            if(!in.good())
                fatalError("Bad ppm file");

            c.r = (float) r / maxColor;
            c.g = (float) g / maxColor;
            c.b = (float) b / maxColor;
            data.at(i).push_back(c);
        }
    }

    in.close();
}
