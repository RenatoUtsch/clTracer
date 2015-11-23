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

#ifndef INTERSECTION_CL
#define INTERSECTION_CL

/// Value returned by the trace function.
typedef enum IntersectionType {
    NoIntersection,
    SphereIntersection,
    PolyhedronIntersection
} IntersectionType;

/**
 * Traces the ray cast by sample() and sees if it intersects anything. Returns
 * what happened.
 * @param origin The ray origin.
 * @param direction The ray direction.
 * @param exclID ID of an object to be excluded from the search. Set to -1 to
 * search all objects.
 * @param exclType Type of an object to be excluded from the search. Set to
 * 0 to search all objects.
 * @param endPos If the object is found after this distance, it will not be
 * counted. Set to 0 to search for all objects.
 * @param outIntersectionID Set to the ID of the intersected object. Set to
 * 0 to ignore.
 * @param outIntersection Set to the point of intersection. Set to 0 to ignore.
 * @param outIntersectionNormal Set to the normal at the point of intersection.
 * Set to 0 to ignore.
 * @param outInside Set to true if the ray is inside (the sphere) and to false
 * otherwise. Set to 0 to ignore.
 * @return The type of intersection.
 */
IntersectionType trace(float4 origin, float4 direction,
        IntersectionType exclType, int exclID, float4 *endPos,
        int *outIntersectionID, float4 *outIntersection,
        float4 *outIntersectionNormal, bool *outInside);

/**
 * Tries to intersect with a sphere.
 * @param origin Origin of the ray.
 * @param dir Direction of the ray.
 * @param maxT Maximum parametric value. If the intersection generates a bigger
 * parametric value, it is discarded.
 * @param inside Set to true if the intersection is inside the sphere and to
 * false otherwise.
 * @return The parametric value used to calculate the intersection position.
 */
float sphereIntersection(float4 origin, float4 dir, float4 center,
        float radius2, float maxT, bool *inside);

/**
 * Tries to instersect with a polyhedron.
 * @param id ID of the polyhedron to try to intersect.
 * @param origin Origin of the ray.
 * @param dir Direction of the ray.
 * @param maxT Maximum parametric value. If the intersection generates a bigger
 * parametric value, it is discarded.
 * @param normal Set to the plane's normal.
 * @return The parametric value used to calculate the intersection position.
 */
float polyhedronIntersection(int id, float4 origin, float4 dir, float maxT,
        float4 *normal);

IntersectionType trace(float4 origin, float4 direction,
        IntersectionType exclType, int exclID, float4 *endPos,
        int *outIntersectionID, float4 *outIntersection,
        float4 *outIntersectionNormal, bool *outInside)
{
    float4 closestNormal;
    float closestT = FLT_MAX;
    int closestID;
    IntersectionType closestType = NoIntersection;
    bool closestInside = false;
    bool inside;

    // Find the maximum t.
    // If origin + t * dir = endPos, then t = (endPos - origin) / dir.
    float maxT;
    if(endPos)
        maxT = (endPos->x - origin.x) / direction.x;
    else
        maxT = FLT_MAX;

    // Intersect with spheres.
    for(int i = 0; i < NumSpheres; ++i) {
        if(exclType == SphereIntersection && i == exclID) continue;

        float t = sphereIntersection(origin, direction, spheres[i].center,
                spheres[i].radius2, maxT, &inside);

        if(t > FLT_EPSILON && t < closestT) {
            closestT = t;
            closestType = SphereIntersection;
            closestID = i;
            closestInside = inside;
        }
    }
    // Intersect with polyhedrons.
    for(int i = 0; i < NumPolyhedrons; ++i) {
        if(exclType == PolyhedronIntersection && i == exclID) continue;

        float4 normal;
        float t = polyhedronIntersection(i, origin, direction, maxT, &normal);

        if(t > FLT_EPSILON && t < closestT) {
            closestT = t;
            closestType = PolyhedronIntersection;
            closestID = i;
            closestNormal = normal;
        }
    }

    if(closestType != NoIntersection) {
        if(outIntersectionID)
            *outIntersectionID = closestID;
        if(outIntersection)
            *outIntersection = origin + closestT * direction;

        if(closestType == SphereIntersection) {
            if(outIntersectionNormal) {
                closestNormal = normalize(*outIntersection
                        - spheres[closestID].center);
                if(closestInside) // Invert the normal.
                    closestNormal *= -1.0f;

                *outIntersectionNormal = closestNormal;
            }
            if(outInside)
                *outInside = closestInside;

            return SphereIntersection;
        }

        if(closestType == PolyhedronIntersection) {
            if(outIntersectionNormal)
                *outIntersectionNormal = closestNormal;

            if(outInside)
                *outInside = false;

            return PolyhedronIntersection;
        }
    }

    return NoIntersection;
}

float sphereIntersection(float4 origin, float4 dir, float4 center,
        float radius2, float maxT, bool *inside)
{
    float4 e = center - origin;
    float tca = dot(e, dir);
    float d2 = dot(e, e) - pow(tca, 2);
    if(d2 > radius2) return -1.0f; // No intersection.

    float thc = sqrt(radius2 - d2);
    float t1 = tca - thc;
    float t2 = tca + thc;

    // t1 is always smaller than t2 since t1 subtracts thc.
    if(t1 > 0.0f && t1 < maxT) {
        *inside = false;
        return t1;
    }
    else if(t2 > 0.0f && t2 < maxT) {
        *inside = true;
        return t2;
    }

    return -1.0f;
}

float polyhedronIntersection(int id, float4 origin, float4 dir, float maxT,
        float4 *normal) {
    int facesBegin = polyhedrons[id].facesBegin;
    float t;
    float t0 = 0.0f, t1 = FLT_MAX;
    float4 nT0, nT1;

    for(int i = 0; i < polyhedrons[id].numFaces; ++i) {
        float4 p0 = (float4) (origin.xyz, 0.0f);
        float4 n = (float4) (polyhedronFaces[facesBegin + i].xyz, 0.0f);
        float dn = dot(dir, n); // hu
        float val = dot(p0, n) + polyhedronFaces[facesBegin + i].w; // hp

        if(fabs(dn) <= FLT_EPSILON) {
            if(val > FLT_EPSILON)
                t1 = -1.0f;
        }
        if(dn > FLT_EPSILON) {
            t = -val / dn;
            if(t < t1) {
                // Replace the furthest point.
                t1 = t;
                nT1 = n;
            }
        }
        if(dn < -FLT_EPSILON) {
            t = -val / dn;
            if(t > t0) {
                t0 = t;
                nT0 = n;
            }
        }
    }

    if(t1 < t0)
        return -1.0f;
    if(fabs(t0) <= FLT_EPSILON && (t1 >= t0) && t1 < FLT_MAX) {
        *normal = normalize(-1.0f * nT1);
        if(t1 < maxT)
            return t1;
        else
            return -1.0f;
    }
    if(t0 > FLT_EPSILON && t1 >= t0) {
        *normal = normalize(nT0);
        if(t0 < maxT)
            return t0;
        else
            return -1.0f;
    }

    return -1.0f;
}

#endif // !INTERSECTION_CL

