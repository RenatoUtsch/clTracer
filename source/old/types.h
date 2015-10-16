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

#ifndef HOST_TYPES_H
#define HOST_TYPES_H

#include "vecmath.h"

/**
 * This structure defines the camera. All vectors should be normalized.
 */
typedef struct Camera {
    Point pos;  /// Position of the camera.
    Vector dir; /// Direction of the camera.
    Vector up;  /// Up vector of the camera.
    float fovy; /// Field of view of the camera, in degrees.
} Camera;

/**
 * This structure sets info about the display.
 */
typedef struct Screen {
    Point center;           /// Center of the display screen.
    Vector rightDir;        /// Direction to the right of the screen.
    Vector bottomDir;       /// Direction to the bottom of the screen.
    Point topLeftPixel;     /// Position of the top left pixel.
    int widthInPixels;      /// Width of the screen in pixels.
    int heightInPixels;     /// Height of the screen in pixels.
    float width;            /// Width of the screen in world coordinates.
    float height;           /// Height of the screen in world coordinates.
    float pixelWidth;       /// Width of a pixel in world coordinates.
    float pixelHeight;      /// Height of a pixel in world coordinates.

} Screen;

/*
 * --------------------------------------------------------------------------- *
 *                     BEWARE: GATEWAY TO HELL LIES AHEAD                      *
 * --------------------------------------------------------------------------- *
 */

/**
 * This is the World struct. It contains all the information needed about the
 * scene when tracing a ray that is passed to the client.
 */
typedef struct World {
    /**
     * The lights are packed in 8 floats, divided as:
     * 0-2: coordinates (x, y, z) of the light.
     * 3-5: color (r, g, b).
     * 6-7: linear and quadratic attenuation.
     * The constant attenuation is already subtracted from the light color,
     * so it is not needed.
     * The first light is the ambient. You can ignore its coordinates and
     * attenuation, it's only there to simplify.
     */
    float *lights[8];

    /// Number of lights, first is ambient.
    int numLights;

    /*
     * Textures. There are 3 types of textures, each with their own
     * subscriptions: solid, checker and map.
     */

    /**
     * Solid textures are packed in 4 floats, divided as:
     * 0-2: color of the texture.
     * 3: unused.
     */
    float *solidTextures[4];

    /// Number of solid textures.
    int numSolidTextures;

    /**
     * Checker textures are packed in 8 floats:
     * 0-2: color of the first half of the squares.
     * 3-5: color of the second half of the squares.
     * 6: size of the side of the square.
     * 7: unused.
     */
    float *checkerTextures[8];

    /// Number of checker textures.
    int numCheckerTextures;

    /*
     * Map textures use three variables to store its data.
     *
     * The first variable uses 8 floating point values:
     * 0-2: first point to use to map the texture.
     * 3-5: second point to use to map the texture.
     * 6-7: unused
     *
     * The second variable uses four integers:
     * 0: number of lines in the map array (height).
     * 1: number of columns in the map array (width).
     * 2: subscript of the mapTextureData3 array with the beginning of the map.
     * 3: unused
     *
     * The third variable stores an array of 4 floating point values for each
     * texture, stored as row-major and linearized:
     * 0-2: color of the point.
     * 3: unused.
     *
     * All different maps are linearized together in this variable, and you can
     * access when each one of them begins with mapTextureData2.s2 and then know
     * how much it spans by multiplying mapTextureData2.s0 and
     * mapTextureData2.s1.
     */

    /// First map texture variable.
    float *mapTextureData1[8];

    /// Second map texture variable.
    int *mapTextureData2[4];

    /// Third map texture variable.
    float *mapTextureData3[4];

    /// Number of map textures.
    int numMapTextures;

    /**
     * The materials are packed in 8 floats, divided as:
     * 0: The ambient coefficient.
     * 1: The diffuse coefficient.
     * 2: The specular coefficient.
     * 3: The specular exponent.
     * 4: The reflection coefficient.
     * 5: The transmission coefficient.
     * 6: The refraction rate.
     * 7: Unused.
     */
    float *materials[8];

    /// Number of materials.
    int numMaterials;

    /*
     * Sphere objects.
     * The spheres are packed in two parameters: the first are floating points
     * and the second are integers.
     */

    /**
     * The floating points needed by sphere objects are packed in 4 floats:
     * 0-2: position of the center of the sphere.
     * 3: radius of the sphere.
     */
    float *sphereFloatData[4];

    /**
     * The integers needed by sphere objects are packed in 4 shorts:
     * 0: texture type.
     * 1: texture id.
     * 2: material id.
     * 3: unused.
     */
    int *sphereIntData[4];

    /// Number of sphere objects.
    int numSphereObjects;

    /*
     * Polyhedron objects.
     * The polyhedron objects are packed in two parameters: the first are
     * floating points and the second are integers.
     */

    /**
     * The floating points needed by polyhedron objects are packed in groups
     * of 4 floats. Each element has vectors of 4 floats that represent the
     * faces of the polyhedron. These 4 floats are the a, b, c and d
     * coefficients of the ax + by + cz = d equation of the face's plane.
     */
    float *polyhedronFloatData[4];

    /**
     * The integers needed by polyhedron objects are packed in 8 ints:
     * 0: texture type.
     * 1: texture id.
     * 2: material id.
     * 3: number of faces.
     * 4: subscript that represents the start of the polyhedron float data.
     * 5-7: unused.
     * Note that you can access when the polyhedron faces begin by accessing
     * the element number 4 of this vector and how many faces it has by
     * accessing the element number 3.
     */
    int *polyhedronIntData[8];

    /// Number of polyhedron objects.
    int numPolyhedronObjects;
} World;

/**
 * This struct stores all the data needed for the ray tracing algorithm in one
 * place.
 */
typedef struct Scene {
    char *input;        /// Input filename.
    char *output;       /// Output filename.
    Camera camera;      /// Camera data.
    Screen screen;      /// Screen data.
    World world;        /// World data.
} Scene;

#endif // !SCENE_H
