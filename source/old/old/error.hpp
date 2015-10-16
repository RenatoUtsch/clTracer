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

#ifndef ERROR_HPP
#define ERROR_HPP

/**
 * Exits with fatal error.
 * You should call the macro fatalError for easier usage of this function.
 * @param file The file of the error.
 * @param line The line of the error.
 * @param format The error message. A \n will already appear at the end and
 * the line and file of the error also. You can use the printf() syntax
 * with this message.
 * @param ... printf() parameters.
 **/
void fatalErrorFunc(const char *file, int line, const char *format, ...);

/**
 * This function serves as a wrapper for fatalErrorFunc to allow easy insertion
 * of __FILE__ and __LINE__ macros to ease the debugging process.
 * @param format The error message. A \n will already be appended at the end
 * and the line and file of the error also. You can use the printf() syntax
 * with this message.
 * @param .. printf() parameters.
 **/
#define fatalError(...) fatalErrorFunc(__FILE__, __LINE__, __VA_ARGS__)

#endif // !ERROR_HPP
