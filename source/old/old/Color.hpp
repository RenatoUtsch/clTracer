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

#ifndef COLOR_HPP
#define COLOR_HPP

#include <iostream>

/// Color.
class Color {

public:
    /// Red component. Between 0 and 1.
    float r;

    /// Green component. Between 0 and 1.
    float g;

    /// Blue component. Between 0 and 1.
    float b;

    Color(float _r = 0.0f, float _g = 0.0f, float _b = 0.0f)
        : r(_r), g(_g), b(_b) { }

    /// Clamps to a maximum of 1.0f.
    Color &clamp() {
        if(r > 1.0f)
            r = 1.0f;
        if(g > 1.0f)
            g = 1.0f;
        if(b > 1.0f)
            b = 1.0f;
        return *this;
    }

    Color &operator+=(float other) {
        r += other;
        g += other;
        b += other;
        return *this;
    }

    Color &operator+=(const Color &other) {
        r += other.r;
        g += other.g;
        b += other.b;
        return *this;
    }

    Color &operator*=(float other) {
        r *= other;
        g *= other;
        b *= other;
        return *this;
    }

    Color &operator*=(const Color &other) {
        r *= other.r;
        g *= other.g;
        b *= other.b;
        return *this;
    }
};

inline Color operator+(Color left, float right) {
    left += right;
    return left;
}

inline Color operator+(Color left, const Color &right) {
    left += right;
    return left;
}

inline Color operator*(Color left, float right) {
    left *= right;
    return left;
}

inline Color operator*(Color left, const Color &right) {
    left *= right;
    return left;
}

inline std::ostream &operator<<(std::ostream &os, const Color &color) {
    os << (int) (color.r * 255) << " " << (int) (color.g * 255) << " "
        << (int) (color.b * 255) << std::endl;
    return os;
}

#endif // !COLOR_HPP
