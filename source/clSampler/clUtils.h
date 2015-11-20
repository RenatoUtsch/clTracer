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

#ifndef CLUTILS_H
#define CLUTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "OpenCL.h"

/*
 * Utility functions for OpenCL.
 */

/**
 * Loads the source from file fp to the buffer.
 * @param filename Name of the input file.
 * @param bufferSize Size of the buffer. Reads until bufferSize - 1 and then
 * adds the '\0'. If 0, buffer must be NULL.
 * @param buffer Buffer where the file's contents will be stored. If NULL, this
 * function only fills the fileSize parameter.
 * @param fileSize is set to the size in bytes of the entire file's contents.
 * This size already includes the null-terminator at the end of the string.
 * If NULL, this function only fills the buffer parameter.
 * @return 0 on success or < 0 in case of error.
 */
int cluLoadSource(const char *filename, size_t bufferSize, char *buffer,
        long *fileSize);

/**
 * Builds the given source code to the given device and returns a cl_program
 * pointing to it.
 * Prints the error message in case of failure to stderr.
 * @param options Options to the compiler.
 * @param err 0 in case of success or < 0 in case of failure.
 * @return the compiled program or NULL in case of failure.
 */
cl_program cluBuildProgram(cl_context context, cl_device_id device,
        const char *source, size_t sourceSize, const char *options, int *err);

#ifdef __cplusplus
}
#endif

#endif // !CLUTILS_H
