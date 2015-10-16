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

/// Value returned by the trace function.
typedef enum IntersectionType {
    NoIntersection,
    SphereIntersection,
    PolyhedronIntersection
} IntersectionType;

/**
 * Traces the ray cast by sample() and sees if it intersects anything. Returns
 * what happened.
 */
IntersectionType trace(float *intersectionDistance, int *intersectionId,
        float4* intersectionNormal,
        float4 origin, float4 direction,
        __constant float4 *sphereFloatData, int numSphereObjects,
        __constant float4 *polyhedronFloatData,
        __constant int8 *polyhedronIntData, int numPolyhedronObjects)
{

}

/**
 * Samples a ray from origin through direction.
 * Some parameters are constant and othes global because the number of constant
 * parameters is limited.
 */
__kernel void sample(__global float4 *out,
        __global float4 *origin, __global float4 *direction,
        __constant float8 *lights, int numLights,
        __global float4 *solidTextures, __global float8 *checkerTextures,
        __global float8 *mapTextureData1, __global int4 *mapTextureData2,
        __global float4 *mapTextureData3, __constant float8 *materials,
        __constant float4 *sphereFloatData, __constant int4 *sphereIntData,
        int numSphereObjects, __constant float4 *polyhedronFloatData,
        __constant int8 *polyhedronIntData, int numPolyhedronObjects)
{

}
