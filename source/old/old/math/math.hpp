/*
 * Math library intended for computer graphics, animation, physics and games
 * (but not restricted to it).
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

#ifndef MATH_MATH_HPP
#define MATH_MATH_HPP

/**
 * Proper inclusion of the standard math library and definition of additional
 * types in case they are missing.
 * Please use this header instead of including <cmath>.
 **/

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // !M_PI

/**
 * Converts from degrees to radians.
 **/
inline double toRads(double degrees) {
    return degrees * (M_PI / 180.0);
}

/**
 * Converts from radians to degrees.
 **/
inline double toDegrees(double rads) {
    return rads * (180.0 / M_PI);
}

#endif // !MATH_MATH_HPP
