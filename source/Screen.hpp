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

#ifndef SCREEN_HPP
#define SCREEN_HPP

#include "math/math.hpp"
#include <cstdlib>

/*
 * This class stores the data needed to generate the position and direction of
 * each pixel from a viewpoint.
 * This class also has functions for moving the viewpoint around the scene.
 */
class Screen {
    // Data that is accessible.
    Point _cameraPos;       /// Position of the camera in the world.
    Point _screenCenter;    /// Center of the projection plane.
    Vector _up;             /// Direction of the top of the camera.
    Vector _right;          /// Direction to the right of the camera.
    float _width;           /// Width in world coordinates of the screen.
    float _height;          /// Height in world coordinates of the screen.
    int _widthInPixels;         /// Width of the screen in pixels.
    int _heightInPixels;        /// Height of the screen in pixels.

    // Data that may be recalculated when being accessed.
    float *_linearizedTopLeftPixel;     /// Array with the top left pixel.
    float *_linearizedCameraPos;        /// Array with the camera position.
    float *_linearizedUpVector;         /// Array with the up vector.
    float *_linearizedRightVector;      /// Array with the right vector.
    bool _recalculateTopLeft;   /// If needs to recalculate the position of the
                                /// top left pixel before linearizedTopLeftPixel().
    bool _recalculateCameraPos; /// If needs to recalculate the camera position
                                /// before linearizedCameraPos()
    bool _recalculateUpVector;  /// If needs to recalculate the up vector before
                                /// linearizedUpVector();
    bool _recalculateRightVector; /// If needs to recalculate the right vector
                                /// before linearizedRightVector();

    // Data used for internal calculations.
    float _fovy;                /// Field of view, in degrees.

public:
    /**
     * Initializes the screen.
     * @param aInput Name of the input file with the initial data.
     * @param aWidthInPixels Width of the screen in pixels.
     * @param aHeightInPixels Height of the screen in pixels.
     */
    Screen(const char *aInput, int aWidthInPixels, int aHeightInPixels);

    ~Screen();

    /**
     * Returns the camera position.
     */
    inline const Point &cameraPos() {
        return _cameraPos;
    }

    /**
     * Returns the center of the projection plane.
     */
    inline const Point &screenCenter() {
        return _screenCenter;
    }

    /**
     * Returns a normalized vector that represents the direction where the
     * camera is looking at.
     */
    inline Vector cameraDir() {
        return (_screenCenter - _cameraPos).normalize();
    }

    /**
     * Returns the up vector of the camera and the projection plane.
     */
    inline const Vector &up() {
        return _up;
    }

    /**
     * Returns the right vector of the camera and the projection plane.
     */
    inline const Vector &right() {
        return _right;
    }

    /**
     * Returns the width of the screen in world coordinates.
     */
    inline float width() {
        return _width;
    }

    /**
     * Returns the height of the screen in world coordinates.
     */
    inline float height() {
        return _height;
    }

    /**
     * Returns the width of a pixel in world coordinates.
     */
    inline float pixelWidth() {
        return _width / _widthInPixels;
    }

    /**
     * Returns the height of a pixel in world coordinates.
     */
    inline float pixelHeight() {
        return _height / _heightInPixels;
    }

    /**
     * Returns the number of pixels in the width of the image.
     */
    inline int widthInPixels() {
        return _widthInPixels;
    }

    /**
     * Returns the number of pixels in the height of the image.
     */
    inline int heightInPixels() {
        return _heightInPixels;
    }

    /**
     * Returns the center of the top left pixel.
     */
    inline Point topLeftPixel() {
        return _screenCenter - (_right * (_width / 2))
            + (_up * (_height / 2));
    }

    /**
     * Moves the camera position to the left the given world coordinates.
     */
    void moveLeft(float distance);

    /**
     * Moves the camera position to the right the given world coordinates.
     */
    void moveRight(float distance);

    /**
     * Moves the camera position up the given world coordinates.
     */
    void moveUp(float distance);

    /**
     * Moves the camera position down the given world coordinates.
     */
    void moveDown(float distance);

    /**
     * Rotates the camera to the left the given angle (in degrees).
     */
    void rotateLeft(float degrees);

    /**
     * Rotates the camera to the right the given angle (in degrees).
     */
    void rotateRight(float degrees);

    /**
     * Rotates the camera up the given angle (in degrees
     */
    void rotateUp(float degrees);

    /**
     * Rotates the camera down the given angle (in degrees).
     */
    void rotateDown(float degrees);

    /// Size of a linearized vector.
    const size_t LinearizedVectorSize = 4 * sizeof(float);

    /**
     * If needs to update the top left pixel position.
     */
    inline bool linearizedTopLeftPixelNeedsUpdate() {
        return _recalculateTopLeft;
    }

    /**
     * If needs to update the linearized camera pos.
     */
    inline bool linearizedCameraPosNeedsUpdate() {
        return _recalculateCameraPos;
    }

    /**
     * If needs to update the linearized up vector.
     */
    inline bool linearizedUpVectorNeedsUpdate() {
        return _recalculateUpVector;
    }

    /**
     * If needs to update teh linearized right vector.
     */
    inline bool linearizedRightVectorNeedsUpdate() {
        return _recalculateRightVector;
    }

    /**
     * Returns the linearized array with the position of the top left pixel,
     * using 4 floats.
     */
    const float *linearizedTopLeftPixel();

    /**
     * Returns the linearized array with the linearized position of the camera,
     * using 4 floats.
     */
    const float *linearizedCameraPos();

    /**
     * Returns the linearized array with the up vector, using 4 floats.
     */
    const float *linearizedUpVector();

    /**
     * Returns the linearized array with the right vector, using 4 floats.
     */
    const float *linearizedRightVector();
};

#endif // !SCREEN_HPP
