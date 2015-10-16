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

#ifndef TRACER_H
#define TRACER_H

#include "types.h"

/**
 * Struct that stores the data needed by a tracer (except for the scene data).
 */
typedef struct Tracer Tracer;

/**
 * Creates and returns a new tracer.
 * Expensive call! Has to compile all kernels and etc. You should call this only
 * on the startup.
 * Call destroyTracer() when you don't need the tracer anymore.
 */
Tracer *createTracer();

/**
 * Destroys a created tracer.
 * Without calling this function the tracer will continue using memory and will
 * result at the end in leakage.
 */
void destroyTracer(Tracer *tracer);

/**
 * Calls the trace function of the tracer.
 * Returns the image rendered by the tracer for the current scene.
 */
unsigned char *trace(Tracer *tracer, Scene *scene);

#endif // !TRACER_H
