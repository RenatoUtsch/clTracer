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

#ifndef VECMATH_H
#define VECMATH_H

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // !M_PI

/**
 * Converts from degrees to radians.
 **/
#define toRads(degrees) ((degrees) * (M_PI / 180.0f))

/**
 * Converts from radians to degrees.
 **/
#define toDegrees(rads) ((rads) * (180.0f / M_PI))

/*
 * Utility functions for processing vectors and points on the host.
 */

typedef float Vector[4];    /// Vector type.
typedef float Point[4];     /// Point type.

/// Assigns a to b.
void pointAssign(Point a, Point b);

/// Subtracts two points, creating a vector. c = a - b.
void pointSub(Point a, Point b, Vector c);

/// Adds a vector to a point. c = a + b
void pointVecAdd(Point a, Vector b, Point c);

/// Returns the distance between two points.
float pointDistance(Point a, Point b);

/// Subtracts one vector from the other and saves it in c. c = a - b.
void vecSub(Vector a, Vector b, Vector c);

/// Normalizes v.
void vecNormalize(Vector v);

/// Makes b = a * c
void vecScale(Vector a, float c, Vector b);

/// Returns a dot b.
float vecDot(Vector a, Vector b);

/// Returns the magnitude of v.
float vecMag(Vector v);

/// Makes c = a cross b.
void vecCross(Vector a, Vector b, Vector c);

/// Makes the orthogonal projection of b in relation to a and saves it in c.
void vecProj(Vector a, Vector b, Vector c);


#endif // !VECMATH_H
