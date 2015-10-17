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
 * Tries to intersect with a sphere.
 */
float sphereIntersection(int id, float4 origin, float4 dir, bool *inside) {
    float4 e = (origin - sphereObjects[id].center);

    float a = dot(dir, dir);
    float b = 2.0f * dot(dir, e);
    float c = dot(e, e) - pow(sphereObjects[id].radius, 2);

    float discriminant = pow(b, 2) - 4.0f * a * c;
    if(discriminant < FLT_EPSILON) {
        // No intersection.
        return -1.0f;
    }

    // Solve the equation. Test both solutions.
    discriminant = sqrt(discriminant);
    float t1 = (-b - discriminant) / (2.0f * a);
    float t2 = (-b + discriminant) / (2.0f * a);

    // t1 is always smaller than t2 because it uses -b - discriminant;
    if(t1 > FLT_EPSILON) {
        *inside = false;
        return t1;
    }
    else if(t2 > FLT_EPSILON) {
        *inside = true;
        return t2;
    }

    return -1.0f;
}

/**
 * Traces the ray cast by sample() and sees if it intersects anything. Returns
 * what happened.
 */
IntersectionType trace(float4 origin, float4 direction,
        float *intersectionDistance, int *intersectionID,
        float4* intersectionNormal, bool *inside)
{
    for(int i = 0; i < numSphereObjects; ++i) {
        float t = sphereIntersection(i, origin, direction, inside);
        float4 intersection = origin + t * direction;

        if(t > FLT_EPSILON) {
            *intersectionDistance = t;
            *intersectionID = i;
            *intersectionNormal = normalize(intersection - sphereObjects[i].center);

            if(inside) // Invert the normal.
                *intersectionNormal *= -1.0f;

            return SphereIntersection;
        }
    }

    return NoIntersection;
}

__constant sampler_t imageSampler = CLK_NORMALIZED_COORDS_FALSE
                | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

/**
 * Samples a ray from origin through direction.
 */
__kernel void sample(__global float4 *origin, __global float4 *topLeft,
        __global float4 *up, __global float4 *right, float pixelWidth,
        float pixelHeight, __write_only image2d_t out)
{
    int2 coord = (int2) (get_global_id(0), get_global_id(1));
    uint4 outColor;
    float distance;
    int id;
    float4 normal;
    bool inside;

    // First get the point position.
    float4 dir = *topLeft + (*right * (coord.x * pixelWidth))
        - (*up * (coord.y * pixelHeight));

    // Now make it a direction vector.
    dir -= *origin;
    dir = normalize(dir);

    // See if the ray intersects anything.
    IntersectionType iType = trace(*origin, dir, &distance, &id, &normal, &inside);

    if(iType == NoIntersection) { // Use ambient color.
        write_imageui(out, coord, (uint4) (lights[0].color * 255.0f));
        return;
    }
    else {
        write_imageui(out, coord,
                (uint4) (solidTextures[sphereObjects[id].textureID].color * 255.0f));
        return;
    }

    float ambientCoef = iType == SphereIntersection ?
            materials[sphereObjects[id].materialID].ambientCoef
            : materials[0].ambientCoef;

    float4 ambient = lights[0].color * ambientCoef;
    float4 diffuse, specular;

    // Trace a ray to all light sources.
    for(int i = 0; i < numLights; ++i) {

    }

    write_imageui(out, coord, outColor);
}
