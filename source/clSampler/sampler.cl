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
float sphereIntersection(int id, float4 origin, float4 dir, float maxT,
        bool *inside)
{
    float4 e = (origin - spheres[id].center);

    float a = dot(dir, dir);
    float b = 2.0f * dot(dir, e);
    float c = dot(e, e) - pow(spheres[id].radius, 2);

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
    if(t1 > FLT_EPSILON && t1 < maxT) {
        *inside = false;
        return t1;
    }
    else if(t2 > FLT_EPSILON && t2 < maxT) {
        *inside = true;
        return t2;
    }

    return -1.0f;
}

/**
 * Traces the ray cast by sample() and sees if it intersects anything. Returns
 * what happened.
 * @param origin The ray origin.
 * @param direction The ray direction.
 * @param exclID ID of an object to be excluded from the search. Set to -1 to
 * search all objects.
 * @param endPos If the object is found after this distance, it will not be
 * counted. Set to 0 to search for all objects.
 */
IntersectionType trace(float4 origin, float4 direction, int exclID,
        __constant float4 *endPos,
        int *outIntersectionID, float4 *outIntersection,
        float4 *outIntersectionNormal, bool *outInside)
{
    float closestT = FLT_MAX;
    bool closestInside = false;
    int closestID = -1;
    bool inside;

    // Find the maximum t.
    // If origin + t * dir = endPos, then t = (endPos - origin) / dir.
    float maxT;
    if(endPos)
        maxT = (endPos->x - origin.x) / direction.x;
    else
        maxT = FLT_MAX;

    for(int i = 0; i < numSpheres; ++i) {
        if(i == exclID) continue;

        float t = sphereIntersection(i, origin, direction, maxT, &inside);

        if(t > FLT_EPSILON && t < closestT) {
            closestT = t;
            closestID = i;
            closestInside = inside;
        }
    }

    if(closestID != -1) {
        if(outIntersectionID)
            *outIntersectionID = closestID;
        if(outIntersection)
            *outIntersection = origin + closestT * direction;
        if(outIntersectionNormal) {
            *outIntersectionNormal = normalize(*outIntersection
                    - spheres[closestID].center);

            if(closestInside) // Invert the normal.
                *outIntersectionNormal *= -1.0f;
        }
        if(outInside)
            *outInside = closestInside;

        return SphereIntersection;
    }

    return NoIntersection;
}

/// Returns the color of the given texture type / ID.
float4 getTextureColor(TextureType type, int id) {
    float4 color = (float4) (0.0f);

    switch(type) {
        case SolidTextureType:
            color = solidTextures[id].color;
            break;

        case CheckerTextureType:
            break;

        case MapTextureType:
            break;
    }

    return color;
}


__constant sampler_t imageSampler = CLK_NORMALIZED_COORDS_FALSE
                | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

/**
 * Samples a ray from origin through direction.
 */
__kernel void sample(__constant float4 *origin, __constant float4 *topLeft,
        __constant float4 *up, __constant float4 *right, float pixelWidth,
        float pixelHeight, __write_only image2d_t out)
{
    int2 coord = (int2) (get_global_id(0), get_global_id(1));

    // First get the point position.
    float4 dir = *topLeft + (*right * (coord.x * pixelWidth))
        - (*up * (coord.y * pixelHeight));

    // Now make it a direction vector.
    dir = normalize(dir - *origin);

    // See if the ray intersects anything.
    int id;
    float4 intersection;
    float4 normal;
    bool inside = false;
    IntersectionType iType = trace(*origin, dir, -1, 0, &id,
            &intersection, &normal, &inside);

    if(iType == NoIntersection) { // Use ambient color.
        write_imageui(out, coord, (uint4) (lights[0].color * 255.0f));
        return;
    }

    __constant Material *material = &materials[spheres[id].materialID];
    float4 ambientColor = lights[0].color * material->ambientCoef;
    float4 diffuseColor = (float4) (0.0f);
    float4 specularColor = (float4) (0.0f);

    // Trace a ray to all light sources and calculate the local light component.
    // Start at 1 because not counting ambient light.
    for(int i = 1; i < numLights; ++i) {
        float4 lightDir = normalize(lights[i].pos - intersection);
        float dist = distance(lights[i].pos, intersection);

        // Calculate the light attenuation.
        float att = 1.0f / (lights[i].constantAtt
                + dist * lights[i].linearAtt
                + pow(dist, 2) * lights[i].quadraticAtt);

        // If the feeler ray does not intersect the surface of any object before
        // reaching the light, then the object is not shadowed.
        // If the object intersects, then just use the ambient color and ignore
        // the others.
        if(trace(intersection, lightDir, id, &lights[i].pos, 0, 0, 0, 0) == NoIntersection) {
            // Calculate the diffuse light.
            float cosDiff = dot(lightDir, normal);
            if(cosDiff < FLT_EPSILON) cosDiff = 0.0f;

            diffuseColor += lights[i].color * cosDiff
                * material->diffuseCoef * att;

            // Calculate the specular light.

            // Halfway vector.
            float4 halfway = normalize(lightDir + (-1.0f * dir));

            // Cos of the angle between the halfway and the normal.
            float cosSpec = dot(halfway, normal);
            if(cosSpec < FLT_EPSILON) cosSpec = 0.0f;

            // Shininess.
            cosSpec = pow(cosSpec, material->specularExp);

            specularColor += lights[i].color * cosSpec
                * material->specularCoef * att;
        }
    }

    // Calculate the final color.
    // Ambient and diffuse colors are multiplied, the others need a sum.

    float4 outColor = getTextureColor(spheres[id].textureType,
            spheres[id].textureID);
    outColor *= (ambientColor + diffuseColor);
    outColor += specularColor;

    write_imagef(out, coord, clamp(outColor, 0.0f, 1.0f));
}
