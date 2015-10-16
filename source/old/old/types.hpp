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

#ifndef TYPES_HPP
#define TYPES_HPP

#include "math/Vector.hpp"
#include "math/Plane.hpp"
#include "math/Point.hpp"
#include "Color.hpp"
#include "Ppm.hpp"
#include <vector>

struct Scene;

/**
 * @file
 * @brief Types used by the Ray Tracer.
 **/

/// Camera.
struct Camera {
    /// Position point.
    Point pos;

    /// Direction vector.
    Vector dir;

    /// Up vector.
    Vector up;

    /// Field of view, in degrees.
    float fovy;
};

/// Screen display.
struct Screen {
    /// Coordinate of the center of the display screen.
    Point center;

    /// Size in pixels of the width of the screen.
    size_t widthNumPixels;

    /// Size in pixels of the height of the screen.
    size_t heightNumPixels;

    /// Size in world coordinates of the width of the screen.
    float width;

    /// Size in world coordinates of the height of the screen.
    float height;

    /// Size in world coordinates of the width of a pixel.
    float pixelWidth;

    /// Size in world coordinates of the height of a pixel.
    float pixelHeight;

    /// Direction to the right of the screen.
    Vector rightDir;

    /// Direction to the bottom of the screen.
    Vector bottomDir;

    /// Position of the top left pixel.
    Point topLeft;
};

/// Attenuation factor of the light.
struct Attenuation {
    /// Constant attenuation.
    float constant;

    /// Linear attenuation (with the distance).
    float linear;

    /// Quadratic attenuation (with the distance).
    float quadratic;
};

/// Light.
struct Light {
    Point pos;
    Color color;
    Attenuation attenuation;
};

/// Types of textures.
enum TextureType {
    SolidTextureType,
    CheckerTextureType,
    MapTextureType
};

/// Solid texture data.
struct SolidTextureData {
    /// Color of the entire texture.
    Color color;
};

/// Checker texture data.
struct CheckerTextureData {
    /// Color of the first half of squares.
    Color color1;

    /// Color of the second half of squares.
    Color color2;

    /// Array with the squares.
    Color map[2][2];

    /// Size of the side of the square.
    float size;
};

/// Map texture data.
struct MapTextureData {
    /// Name of the file with the texture.
    std::string filename;

    /// First point to use to map the texture.
    Point p0;

    /// Second point to use to map the texture.
    Point p1;

    /// Data of the texture.
    Ppm ppm;
};

/// A texture.
struct Texture {
    /// Type of the ID of a texture.
    typedef size_t Id;

    /// ID of the texture.
    Id id;

    /// Type of the texture.
    TextureType type;

    /// Data related to a solid texture.
    SolidTextureData solid;

    /// Data related to a checker texture.
    CheckerTextureData checker;

    /// Data related to a map texture.
    MapTextureData map;
};

/// Material.
struct Material {
    /// Type of the ID of a material.
    typedef size_t Id;

    /// ID of the material.
    Id id;

    // Coefficients of the material.
    float ambientCoef;
    float diffuseCoef;
    float specularCoef;
    float specularExp;
    float reflectionCoef;
    float transmissionCoef;
    float refractionRate;
};

/// Types of objects.
enum ObjectType {
    SphereObjectType,
    PolyhedronObjectType
};

/// Data of a sphere object.
struct SphereObjectData {
    /// Position of the sphere.
    Point pos;

    /// Radius of the sphere.
    float radius;
};

/// Data of a polyhedron object.
struct PolyhedronObjectData {
    /// Number of faces.
    size_t numFaces;

    /// Equation of the plane of each face.
    std::vector<Plane> planes;
};

/// Object.
struct Object {
    /// Type of the id of an object.
    typedef size_t Id;

    /// Scene of the object.
    Scene *scene;

    /// ID of the object.
    Id id;

    /// Type of the object.
    ObjectType type;

    /// Texture ID of the object.
    Texture::Id textureId;

    /// Material ID of the object.
    Material::Id materialId;

    /// Returns the texture given the scene.
    const Texture &texture() const;

    /// Returns the material given the scene.
    const Material &material() const;

    /// Returns the color at a given point.
    Color color(const Point &p) const;

    /// Data related to a sphere object.
    SphereObjectData sphere;

    /// Data related to a polyhedron object.
    PolyhedronObjectData polyhedron;
};

#endif // !TYPES_HPP
