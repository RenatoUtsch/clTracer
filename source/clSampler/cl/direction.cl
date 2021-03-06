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

#ifndef DIRECTION_CL
#define DIRECTION_CL

#include "random.cl"

/**
 * Returns the reflection direction.
 * @param dir Ray direction.
 * @param normal Normal at the intersection.
 */
float4 getReflectionDirection(float4 dir, float4 normal);

/**
 * Returns the transmission direction.
 * @param refrRate The refraction rate.
 * @param dir Direction of the ray.
 * @param normal Normal at the intersection.
 * @param transDir Output transmission direction.
 * @return If there is indeed transmission or not.
 */
bool getTransmissionDirection(float refrRate, float4 dir, float4 normal,
        bool inside, float4 *transDir);

float4 getReflectionDirection(float4 dir, float4 normal) {
    // Get the inversion of the direction.
    float c = dot(-1.0f * dir, normal);

    return normalize(dir + (2 * normal * c));
}

bool getTransmissionDirection(float refrRate, float4 dir, float4 normal,
        bool inside, float4 *transDir) {
    float ddn = dot(dir, normal);
    float cos2t = 1.0f - pow(refrRate, 2) * (1.0f - pow(ddn, 2));

    if(cos2t < -FLT_EPSILON) { // Total internal reflection.
        *transDir = normalize((2 * normal * ddn) - dir);
        return true;
    }
    else if(cos2t > FLT_EPSILON) { // Refraction.
        *transDir = normalize(dir * refrRate - normal *
               ((inside ? 1.0f : -1.0f) * ddn * refrRate + sqrt(cos2t)));
        return true;
    }
    else { // Parallel ray.
        return false;
    }
}

#endif // !DIRECTION_CL
