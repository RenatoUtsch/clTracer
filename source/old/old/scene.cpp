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

#include "scene.hpp"
#include "rayTracer.hpp"
#include <fstream>
#include <iostream>
#include <cstdlib>

/**
 * Maximum depth of recursion from the raytracer.
 **/
static const size_t maxDepth = 4;

void computeScene(Scene &scene) {
    // Image file.
    std::ofstream image(scene.output);
    if(!image.is_open()) {
        std::cerr << "Failed to open output file: " << scene.output << std::endl;
        exit(1);
    }

    // PPM header.
    image << "P3" << std::endl;
    image << scene.screen.widthNumPixels << " " << scene.screen.heightNumPixels
        << std::endl;
    image << "255" << std::endl;

    Vector pixelRight = scene.screen.pixelWidth * scene.screen.rightDir;
    Vector pixelBottom = scene.screen.pixelHeight * scene.screen.bottomDir;

    // For each pixel of the virtual screen, write the pixel color.
    for(size_t h = 0; h < scene.screen.heightNumPixels; ++h) {
        for(size_t w = 0; w < scene.screen.widthNumPixels; ++w) {
            // Get the pixel position.
            Point pos = scene.screen.topLeft;
            pos += w * pixelRight;
            pos += h * pixelBottom;

            // Get a vector width the distance between the camera and the pixel.
            Vector d = pos - scene.camera.pos;
            d.normalize();

            Color p = rayTracer(scene, scene.camera.pos, d, maxDepth);
            image << p;
        }
    }

    // Close the image and return.
    image.close();
}
