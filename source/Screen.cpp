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

#include "Screen.hpp"
#include "error.hpp"
#include "math/math.hpp"
#include <fstream>
#include <limits>

Screen::Screen(const CmdArgs &args)
        : _width(args.width()), _height(args.height()) {

    // Read the input file.
    std::string input = args.inputFilename();
    std::ifstream in(input);
    stop_if(!in.is_open(), "failed to open input file (%s).", input.c_str());

    if(args.extSpec())
        in.ignore(std::numeric_limits<std::streamsize>::max(), in.widen('\n'));

    Point camera, center;
    Vector up, right;
    float fovy;
    in >> camera.x >> camera.y >> camera.z;
    in >> center.x >> center.y >> center.z;
    in >> up.x >> up.y >> up.z;
    in >> fovy;

    // Camera direction to the center of the screen..
    Vector dir = (center - camera).normalize();

    // Make up orthogonal to the camera direction.
    up -= Vector::proj(dir, up);
    up.normalize();

    // Calculate the right direction.
    right = Vector::cross(dir, up);

    // Calculate the width and height of the screen.
    float d = Point::distance(center, camera);
    _heightSize = 2 * tan(toRads(fovy / 2.0f)) * d;
    _widthSize = (_heightSize * _width) / _height;

    // Save the top left pixel.
    Point topLeft = center - (right * (_widthSize / 2)) + (up * (_heightSize / 2));
    _topLeftPixelPos[0] = topLeft.x;
    _topLeftPixelPos[1] = topLeft.y;
    _topLeftPixelPos[2] = topLeft.z;
    _topLeftPixelPos[3] = 1.0f;

    _cameraPos[0] = camera.x;
    _cameraPos[1] = camera.y;
    _cameraPos[2] = camera.z;
    _cameraPos[3] = 1.0f;

    _upVector[0] = up.x;
    _upVector[1] = up.y;
    _upVector[2] = up.z;
    _upVector[3] = 0.0f;

    _rightVector[0] = right.x;
    _rightVector[1] = right.y;
    _rightVector[2] = right.z;
    _rightVector[3] = 0.0f;

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

