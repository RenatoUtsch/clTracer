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
#include <limits>

Screen::Screen(const CmdArgs &args)
        : _widthInPixels(args.width()), _heightInPixels(args.height()),
          _aaLevel(args.aaLevel()),
          _recalculateTopLeft(true), _recalculateCameraPos(true),
          _recalculateUpVector(true), _recalculateRightVector(true) {

    _linearizedTopLeftPixel = new float[4];
    _linearizedCameraPos = new float[4];
    _linearizedUpVector = new float[4];
    _linearizedRightVector = new float[4];

    // Read the input file.
    std::string input = args.inputFilename();
    std::ifstream in(input);
    stop_if(!in.is_open(), "failed to open input file (%s).", input.c_str());

    if(args.extSpec())
        in.ignore(std::numeric_limits<std::streamsize>::max(), in.widen('\n'));

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
    delete [] _linearizedTopLeftPixel;
    delete [] _linearizedCameraPos;
    delete [] _linearizedUpVector;
    delete [] _linearizedRightVector;
}

const float *Screen::linearizedTopLeftPixel() {
    if(_recalculateTopLeft) {
        Point topLeft = topLeftPixel();
        _linearizedTopLeftPixel[0] = topLeft.x;
        _linearizedTopLeftPixel[1] = topLeft.y;
        _linearizedTopLeftPixel[2] = topLeft.z;
        _linearizedTopLeftPixel[3] = 1.0f;
        _recalculateTopLeft = false;
    }

    return _linearizedTopLeftPixel;
}

const float *Screen::linearizedCameraPos() {
    if(_recalculateCameraPos) {
        _linearizedCameraPos[0] = _cameraPos.x;
        _linearizedCameraPos[1] = _cameraPos.y;
        _linearizedCameraPos[2] = _cameraPos.z;
        _linearizedCameraPos[3] = 1.0f;
        _recalculateCameraPos = false;
    }

    return _linearizedCameraPos;
}

const float *Screen::linearizedUpVector() {
    if(_recalculateUpVector) {
        _linearizedUpVector[0] = _up.x;
        _linearizedUpVector[1] = _up.y;
        _linearizedUpVector[2] = _up.z;
        _linearizedUpVector[3] = 0.0f;

        _recalculateUpVector = false;
    }

    return _linearizedUpVector;
}

const float *Screen::linearizedRightVector() {
    if(_recalculateRightVector) {
        _linearizedRightVector[0] = _right.x;
        _linearizedRightVector[1] = _right.y;
        _linearizedRightVector[2] = _right.z;
        _linearizedRightVector[3] = 0.0f;

        _recalculateRightVector = false;
    }

    return _linearizedRightVector;
}
