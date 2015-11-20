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

#include "math/math.hpp"
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
    // Data that is accessible.
    Point _cameraPos;       /// Position of the camera in the world.
    Point _screenCenter;    /// Center of the projection plane.
    Vector _up;             /// Direction of the top of the camera.
    Vector _right;          /// Direction to the right of the camera.
    float _width;           /// Width in world coordinates of the screen.
    float _height;          /// Height in world coordinates of the screen.
    int _widthInPixels;     /// Width of the screen in pixels.
    int _heightInPixels;    /// Height of the screen in pixels.
    int _aaLevel;           /// Antialiasing level.

    // Data that may be recalculated when being accessed.
    float _linearizedTopLeftPixel[4];      /// Array with the top left pixel.
    float _linearizedCameraPos[4];         /// Array with the camera position.
    float _linearizedUpVector[4];          /// Array with the up vector.
    float _linearizedRightVector[4];       /// Array with the right vector.

    // Data used for internal calculations.
    float _fovy;                /// Field of view, in degrees.

public:
    /**
     * Initializes the screen.
     */
    Screen(const CmdArgs &args);

    /**
     * Returns the camera position.
     */
    inline const Point &cameraPos() const {
        return _cameraPos;
    }

    /**
     * Returns the center of the projection plane.
     */
    inline const Point &screenCenter() const {
        return _screenCenter;
    }

    /**
     * Returns a normalized vector that represents the direction where the
     * camera is looking at.
     */
    inline Vector cameraDir() const {
        return (_screenCenter - _cameraPos).normalize();
    }

    /**
     * Returns the up vector of the camera and the projection plane.
     */
    inline const Vector &up() const {
        return _up;
    }

    /**
     * Returns the right vector of the camera and the projection plane.
     */
    inline const Vector &right() const {
        return _right;
    }

    /**
     * Returns the width of the screen in world coordinates.
     */
    inline float width() const {
        return _width;
    }

    /**
     * Returns the height of the screen in world coordinates.
     */
    inline float height() const {
        return _height;
    }

    /**
     * Returns the width of a pixel in world coordinates.
     */
    inline float pixelWidth() const {
        return _width / _widthInPixels;
    }

    /**
     * Returns the height of a pixel in world coordinates.
     */
    inline float pixelHeight() const {
        return _height / _heightInPixels;
    }

    /**
     * Returns the number of pixels in the width of the image.
     */
    inline int widthInPixels() const {
        return _widthInPixels;
    }

    /**
     * Returns the number of pixels in the height of the image.
     */
    inline int heightInPixels() const {
        return _heightInPixels;
    }

    /**
     * Returns the center of the top left pixel.
     */
    inline Point topLeftPixel() const {
        Point p = _screenCenter - (_right * (_width / 2))
            + (_up * (_height / 2));

        // Get the point with the antialiasing position.
        if(_aaLevel > 1) {
            float pw = pixelWidth(), ph = pixelHeight();
            p += _right * (pw / (2 * _aaLevel) - pw / 2);
            p += _up * (ph / 2 - ph / (2 * _aaLevel));
        }

        return p;
    }

    /// Size of a linearized vector.
    const size_t LinearizedVectorSize = 4 * sizeof(float);

    /**
     * Returns the linearized array with the position of the top left pixel,
     * using 4 floats.
     */
    inline const float *linearizedTopLeftPixel() const {
        return _linearizedTopLeftPixel;
    }

    /**
     * Returns the linearized array with the linearized position of the camera,
     * using 4 floats.
     */
    const float *linearizedCameraPos() const {
        return _linearizedCameraPos;
    }

    /**
     * Returns the linearized array with the up vector, using 4 floats.
     */
    const float *linearizedUpVector() const {
        return _linearizedUpVector;
    }

    /**
     * Returns the linearized array with the right vector, using 4 floats.
     */
    const float *linearizedRightVector() const {
        return _linearizedRightVector;
    }
};

#endif // !SCREEN_HPP
