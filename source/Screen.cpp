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

#include "Screen.hpp"
#include "error.hpp"
#include <fstream>

Screen::Screen(const char *aInput, int aWidthInPixels, int aHeightInPixels)
        : _widthInPixels(aWidthInPixels), _heightInPixels(aHeightInPixels),
          _recalculatePixels(true), _recalculateCameraPos(true) {

    _linearizedPixelPositions = new float[_widthInPixels * _heightInPixels * 4];
    _linearizedCameraPos = new float[4];

    // Read the input file.
    std::ifstream in(aInput);
    stop_if(!in.is_open(), "failed to open input file (%s).", aInput);

    in >> _cameraPos.x >> _cameraPos.y >> _cameraPos.z;
    in >> _screenCenter.x >> _screenCenter.y >> _screenCenter.z;
    in >> _up.x >> _up.y >> _up.z;
    in >> _fovy;

    // Make up orthogonal to the camera direction.
    _up -= Vector::proj(cameraDir(), _up);
    _up.normalize();

    // Calculate the right direction.
    _right = Vector::cross(cameraDir(), _up);

    // Calculate the width and height of the screen.
    float d = Point::distance(_screenCenter, _cameraPos);
    _height = 2 * tan(toRads(_fovy / 2.0f)) * d;
    _width = (_height * _widthInPixels) / _heightInPixels;

    // If the distance from the camera to the center of the screen is d and
    // half the fovy is f, the maximum height of the screen is:
    // A schematic (from the side);
    //                            .
    //                         .  |
    //                     .      | <- h
    //                  .         |
    //               .  f         |
    // Camera -> .----------------. <- Screen center
    //               .     ^d     |
    //                  .         |
    //                     .      | <- h
    //                         .  |
    //                            .
    // tan(f) = h/d => h = tan(f) * d;
    // The height of the screen is h = tan(f) * d.
    //
    // The we know that if the screen has n pixels from top to low (2h), then
    // 2h ~ n
    // The width 2w of the screen, with m pixels, is:
    // 2w ~ m
    // Then 2w = (2h*m)/n
}

Screen::~Screen() {
    delete [] _linearizedPixelPositions;
    delete [] _linearizedCameraPos;
}
