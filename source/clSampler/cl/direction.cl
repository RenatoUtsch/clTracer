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
        float4 *transDir);

/**
 * Returns the light position taking into account its area.
 * @param orig The ray origin.
 * @param id The index of the light source.
 * @param seed Random seed.
 * @return The position of the light.
 */
float4 getRandomLightPos(float4 *orig, int id, uint2 *seed);

float4 getReflectionDirection(float4 dir, float4 normal)
{
    // Get the inversion of the direction.
    float c = dot(-1.0f * dir, normal);

    return normalize(dir + (2 * normal * c));
}

bool getTransmissionDirection(float refrRate, float4 dir, float4 normal,
        float4 *transDir)
{
    float4 invDir = -1.0f * dir;
    float c1 = dot(invDir, normal);
    float c2 = 1.0f - pow(refrRate, 2) * (1.0f - pow(c1, 2));
    float marreta = 3.0f;

    if(c2 < -FLT_EPSILON) { // Total internal reflection.
        *transDir = normalize((2 * normal * c1) - invDir);
        return true;
    }
    else if(c2 > FLT_EPSILON) { // Refraction.
        c2 = sqrt(c2 + marreta); // Marretagem.
        *transDir = normalize(normal * (refrRate * c1 - c2) + dir * refrRate);
        return true;
    }
    else { // Parallel ray.
        return false;
    }
}

float4 getRandomLightPos(float4 *orig, int id, uint2 *seed) {
    float4 pos = lights[id].pos;
    float4 dir = normalize(pos - *orig);

    // We use dir, which is the pos plane normal and the point pos to calculate
    // another point in the plane and use it as the right vector.
    // dir.s0 * pos.x + dir.s1 * pos.y + dir.s2 * pos.z = d
    float d = dot(dir, pos);
    // Now find a random point in the plane.
    float4 point = (float4) (0.0f, 0.0f, 0.0f, 1.0f);
    if(dir.s0) point.s0 = d / dir.s0;
    else if(dir.s1) point.s1 = d / dir.s1;
    else /* if(dir.s2) */ point.s2 = d / dir.s2;

    // Now use the random point as the right direction.
    float4 right = normalize(point - pos);
    float4 up = cross(right, dir);

    // Return a random point in the light.
    return randcircle(pos, right, up, lights[id].radius, seed);
}

#endif // !DIRECTION_CL
