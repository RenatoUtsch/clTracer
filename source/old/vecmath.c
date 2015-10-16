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

#include "vecmath.h"

void pointAssign(Point a, Point b) {
    b[0] = a[0];
    b[1] = a[1];
    b[2] = a[2];
}

void pointSub(Point a, Point b, Vector c) {
    c[0] = a[0] - b[0];
    c[1] = a[1] - b[1];
    c[2] = a[2] - b[2];
}

void pointVecAdd(Point a, Vector b, Point c) {
    c[0] = a[0] + b[0];
    c[1] = a[1] + b[1];
    c[2] = a[2] + b[2];
}

float pointDistance(Point a, Point b) {
    return (float) sqrt(vecDot(a, b));
}

void vecSub(Vector a, Vector b, Vector c) {
    c[0] = a[0] - b[0];
    c[1] = a[1] - b[1];
    c[2] = a[2] - b[2];
}

void vecNormalize(Vector v) {
    float mag = vecDot(v, v);

    v[0] /= mag;
    v[1] /= mag;
    v[2] /= mag;
}

void vecScale(Vector a, float c, Vector b) {
    b[0] = a[0] * c;
    b[1] = a[1] * c;
    b[2] = a[2] * c;
}

float vecDot(Vector a, Vector b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

float vecMag(Vector v) {
    return sqrt(vecDot(v, v));
}

void vecCross(Vector a, Vector b, Vector c) {
    c[0] = a[1] * b[2] - a[2] * b[1];
    c[1] = a[2] * b[0] - a[0] * b[2];
    c[2] = a[0] * b[1] - a[1] * b[0];
}

void vecProj(Vector a, Vector b, Vector c) {
    float dot = vecDot(a, b);
    float mag2 = vecDot(a, a);
    float factor = dot / mag2;

    c[0] = a[0] * factor;
    c[1] = a[1] * factor;
    c[2] = a[2] * factor;
}
