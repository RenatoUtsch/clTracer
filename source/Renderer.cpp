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

#include "Renderer.hpp"
#include "Sampler.hpp"
#include "error.hpp"
#include <GLFW/glfw3.h>

static void errorCallback(int error, const char *description) {
    stop_if(1, "GLFW error %d: %s", error, description);
}

static void keyCallback(GLFWwindow *window, int key, int scancode, int action,
        int mods) {
    // Do something?
}

Renderer::Renderer(const World &world, Screen &screen, const CmdArgs &args) {
    _screen = &screen;

    initGL(args);
    initTexture(args);

    _sampler = new Sampler {world, screen, args, _texture};
}

Renderer::~Renderer() {
    delete _sampler;
    finishGL();
}

void Renderer::initGL(const CmdArgs &args) {
    // Init glfw.
    stop_if(!glfwInit(), "Failed to init GLFW.");

    // Create the window.
    _window = glfwCreateWindow(args.width(), args.height(),
            "Realtime OpenCL Raytracer by Renato Utsch", NULL, NULL);
    stop_if(!_window, "Failed to create a window.");

    glfwSetWindowPos(_window, 100, 100);
    glfwMakeContextCurrent(_window);

    // Setup the callbacks.
    glfwSetErrorCallback(errorCallback);
    glfwSetKeyCallback(_window, keyCallback);

    // Set up OpenCL specifics.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void Renderer::initTexture(const CmdArgs &args) {
    glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, args.width(), args.height(), 0,
            GL_RGBA, GL_FLOAT, NULL);
}

void Renderer::finishGL() {
    glfwDestroyWindow(_window);
    glfwTerminate();
}

void Renderer::render() {
    glClear(GL_COLOR_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, _texture);

    glBegin(GL_QUADS);
        glTexCoord2i(0, 1);
        glVertex3f(-1.0f, 1.0f, 0.0f);
        glTexCoord2i(1, 1);
        glVertex3f( 1.0f, 1.0f, 0.0f);
        glTexCoord2i(1, 0);
        glVertex3f( 1.0f,-1.0f, 0.0f);
        glTexCoord2i(0, 0);
        glVertex3f(-1.0f,-1.0f, 0.0f);
    glEnd();
    glFinish();

    glfwSwapBuffers(_window);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::run() {
    // Run the renderer at max fps until a close event is issued.
    while(!glfwWindowShouldClose(_window)) {
        _sampler->updateScreen(*_screen);
        _sampler->sample();

        render();
    }
}
