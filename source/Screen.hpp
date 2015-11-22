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

#ifndef SCREEN_HPP
#define SCREEN_HPP

#include "CmdArgs.hpp"
#include <cstdlib>
#include <string>

/*
 * This class stores the data needed to generate the position and direction of
 * each pixel from a viewpoint.
 * This class also has functions for moving the viewpoint around the scene.
 * TODO: This class needs a refactor. It needs to return only float* values.
 */
class Screen {
    int _width;                 /// Width of the screen in pixels.
    int _height;                /// Height of the screen in pixels.
    float _widthSize;           /// Width in world coordinates of the screen.
    float _heightSize;          /// Height in world coordinates of the screen.
    float _topLeftPixelPos[4];  /// Position of the top left pixel.
    float _cameraPos[4];        /// Position of the camera.
    float _upVector[4];         /// Direction to the top of the camera.
    float _rightVector[4];      /// Direction to the right of the camera.

public:
    /**
     * Initializes the screen.
     */
    Screen(const CmdArgs &args);

    /**
     * Returns the width of the screen in world coordinates.
     */
    inline float widthSize() const {
        return _widthSize;
    }

    /**
     * Returns the height of the screen in world coordinates.
     */
    inline float heightSize() const {
        return _heightSize;
    }

    /**
     * Returns the width of a pixel in world coordinates.
     */
    inline float pixelWidth() const {
        return _widthSize / _width;
    }

    /**
     * Returns the height of a pixel in world coordinates.
     */
    inline float pixelHeight() const {
        return _heightSize / _height;
    }

    /**
     * Returns the number of pixels in the width of the image.
     */
    inline int width() const {
        return _width;
    }

    /**
     * Returns the number of pixels in the height of the image.
     */
    inline int height() const {
        return _height;
    }

    /// Size of an array from this class.
    const size_t ArraySize = 4 * sizeof(float);

    /**
     * Returns the array with the position of the top left pixel, using 4 floats.
     */
    inline const float *topLeftPixelPos() const {
        return _topLeftPixelPos;
    }

    /**
     * Returns the array with the linearized position of the camera, using 4
     * floats.
     */
    const float *cameraPos() const {
        return _cameraPos;
    }

    /**
     * Returns the array with the up vector, using 4 floats.
     */
    const float *upVector() const {
        return _upVector;
    }

    /**
     * Returns the array with the right vector, using 4 floats.
     */
    const float *rightVector() const {
        return _rightVector;
    }
};

#endif // !SCREEN_HPP
