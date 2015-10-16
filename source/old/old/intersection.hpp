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

#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include "types.hpp"
#include "scene.hpp"
#include "math/Point.hpp"
#include "math/Vector.hpp"

/**
 * Calculates the intersection of the ray with the object and returns it.
 * @param scene The scene where the objects are.
 * @param origin The origin of the ray.
 * @param dir The direction of the ray.
 * @param intersection Set with the point of intersection.
 * @param intersectionColor Set with the color of the object at the point of
 * intersection.
 * @param normal The normal vector of the intersected point. It is normalized.
 * @param intersectedObj Set with the object that was intersected.
 * @param endPos The end position after where a collision is not tested. If NULL,
 * the default, tests all the ray direction.
 * @param exclObj Object to not try for intersection. Defaults to NULL
 * @param inside If is inside of the object.
 * @return true if an intersection was found. If false, there are no intersections
 * and none of the variables were set.
 **/
bool intersection(Scene &scene, const Point &origin, const Vector &dir,
        Point &intersection, Color &intersectionColor, Vector &normal,
        Object &intersectedObj, const Point *endPos = NULL, const Object *exclObj = NULL,
        bool *inside = NULL);

#endif // !INTERSECTION_HPP
