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

#include "intersection.hpp"
#include "math/Plane.hpp"
#include <vector>
#include <cfloat>
#include <cstdlib>

/**
 * Intersection with spheres.
 * Returns the coefficient to multiply by the unit vector dir and sum to
 * origin to get the intersection point.
 **/
static float sphereIntersection(const Object &obj, const Point &origin,
        const Vector &dir, const Point *endPos = NULL, bool *inside = NULL) {
    Vector e = (origin - obj.sphere.pos);

    double a = Vector::dot(dir, dir);
    double b = 2.0f * Vector::dot(dir, e);
    double c = Vector::dot(e, e) - std::pow(obj.sphere.radius, 2);

    double discriminant = std::pow(b, 2) - 4*a*c;
    if(discriminant < FLT_EPSILON) {
        // No intersection.
        return -1.0f;
    }

    // Resolving the equation.
    discriminant = std::sqrt(discriminant);

    // Test both solutions.
    double t1 = (-b - discriminant) / (2.0f * a);
    double t2 = (-b + discriminant) / (2.0f * a);

    // Calculate the maximum t.
    // if origin + t * dir = endPos, then t = (endPos - origin) / dir.
    double maxT;
    if(endPos)
        maxT = (endPos->x - origin.x) / dir.x;
    else
        maxT = FLT_MAX;

    // t1 is always smaller than t2 because it uses -b -discriminant.

    if(t1 > FLT_EPSILON && t1 < maxT) {
        if(inside)
            *inside = false;
        return t1;
    }

    else if(t2 > FLT_EPSILON && t2 < maxT) {
        if(inside)
            *inside = true;
        return t2;
    }
    else
        return -1.0f;
}

/**
 * Intersection with polyhedrons.
 **/
static float polyhedronIntersection(const Object &obj, const Point &origin,
        const Vector &dir, Vector &normal, const Point *endPos = NULL) {
    float t;
    float t0 = 0.0f, t1 = FLT_MAX;
    Point p;
    Vector nT0, nT1;
    const std::vector<Plane> &planes = obj.polyhedron.planes;

    /**
     * Go through all the faces. The ray must intercept all the planes
     * of the faces.
     **/
    for(size_t i = 0; i < planes.size(); ++i) {
        Vector p0(origin.x, origin.y, origin.z);
        Vector n = planes[i].normal();
        float dn = Vector::dot(dir, n); // hu
        float val = Vector::dot(p0, n) + planes[i].d; // hp

        if(dn <= FLT_EPSILON && dn >= -FLT_EPSILON) {
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

    // Calculate the maximum t.
    // if origin + t * dir = endPos, then t = (endPos - origin) / dir.
    double maxT;
    if(endPos)
        maxT = (endPos->x - origin.x) / dir.x;
    else
        maxT = FLT_MAX;

    if(t1 < t0)
        return -1.0f;
    if(std::abs(t0) <= FLT_EPSILON && (t1 >= t0) && t1 < FLT_MAX) {
        normal = (nT1 * (-1)).normalize();
        if(t1 < maxT)
            return t1;
        else
            return -1.0f;
    }
    if(t0 > FLT_EPSILON && t1 >= t0) {
        normal = nT0.normalize();
        if(t0 < maxT)
            return t0;
        else
            return -1.0f;
    }

    return -1.0f;
}

bool intersection(Scene &scene, const Point &origin, const Vector &dir,
        Point &intersection, Color &intersectionColor, Vector &normal,
        Object &intersectedObj, const Point *endPos, const Object *exclObj,
        bool *inside) {
    //
    // TODO: This function turned into a monster.
    //

    // Temporarily store the intersections.
    const Object *closestObj = NULL;
    float closestObjT = FLT_MAX;
    bool closestInside = false;
    bool i;
    float t;
    Vector n;
    Vector closestN;

    // Loop through the objects trying to find an intersection.
    for(std::vector<Object>::const_iterator it = scene.objects.begin();
            it != scene.objects.end(); ++it) {

        // If is to exclude an object, do not analyse it.
        if(exclObj)
            if(exclObj->id == it->id)
                continue;

        // Look at the type of the object.
        switch(it->type) {
            case SphereObjectType:
                // Intersection with sphere.
                t = sphereIntersection(*it, origin, dir, endPos, &i);

                if(t > FLT_EPSILON && t < closestObjT) {
                    closestObj = &(*it);
                    closestObjT = t;
                    closestInside = i;
                }
                break;

            case PolyhedronObjectType:
                // Intersection with polyhedron.
                t = polyhedronIntersection(*it, origin, dir, n, endPos);

                if(t > FLT_EPSILON && t < closestObjT) {
                    closestObj = &(*it);
                    closestObjT = t;
                    closestN = n;
                }
                break;
        }
    }

    // If an object was found.
    if(closestObj) {
        intersectedObj = *closestObj;
        intersection = origin + closestObjT * dir;
        intersectionColor = intersectedObj.color(intersection);

        // Calculate the normals.
        switch(intersectedObj.type) {
            case SphereObjectType:
                normal = (intersection - intersectedObj.sphere.pos).normalize();
                if(inside)
                    *inside = false;

                if(closestInside) {
                    normal = normal * (-1);
                    if(inside)
                        *inside = true;
                }

                /*
                // If the origin is inside the sphere the normal is inverted.
                if(Point::distance(origin, intersectedObj.sphere.pos)
                        < intersectedObj.sphere.radius) {
                    normal = normal * (-1);
                    if(inside)
                        *inside = true;
                }
                else {
                    if(inside)
                        *inside = false;
                }
                */
                break;

            case PolyhedronObjectType:
                normal = closestN;
                break;
        }

        return true;
    }

    // No intersections.
    return false;
}
