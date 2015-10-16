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

#ifndef SCENE_HPP
#define SCENE_HPP

#include "types.hpp"

/// Scene: all the information for a scene in a single place.
struct Scene {
    /// Input instruction file.
    char *input;

    /// Output ppm file.
    char *output;

    /// Screen.
    Screen screen;

    /// Camera.
    Camera camera;

    /// Number of lights.
    size_t numLights;

    /// Ambient light.
    Light ambient;

    /// The other lights.
    std::vector<Light> lights;

    /// Number of textures.
    size_t numTextures;

    /// The textures.
    std::vector<Texture> textures;

    /// Number of materials.
    size_t numMaterials;

    /// The materials.
    std::vector<Material> materials;

    /// Number of objects.
    size_t numObjects;

    /// The objects.
    std::vector<Object> objects;
};

// Computes the scene and writes it to the scene.output file.
void computeScene(Scene &scene);

#endif // !SCENE_HPP
