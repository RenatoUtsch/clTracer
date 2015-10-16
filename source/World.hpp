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

#ifndef WORLD_HPP
#define WORLD_HPP

#include "math/math.hpp"
#include "Color.hpp"
#include "PPMImage.hpp"
#include <fstream>
#include <vector>
#include <cstdint>

/**
 * This struct represents the attenuation.
 */
struct Attenuation {
    float linear;
    float quadratic;
};

/**
 * This struct represents a single light.
 */
struct Light {
    Point pos;                  /// Position of the light.
    Color color;                /// Color of the light.
    Attenuation attenuation;    /// Attenuation of the light.
};

/// Texture types.
enum TextureType {
    SolidTextureType,
    CheckerTextureType,
    MapTextureType
};

/**
 * Represents a solid texture.
 */
struct SolidTexture {
    Color color;    /// color of the texture.
};

/**
 * Represents a checker texture.
 */
struct CheckerTexture {
    Color color1;       /// Color of the first half of the squares;
    Color color2;       /// Color of the second half of the squares;
    float size;         /// Size of the side of the square.
};

/**
 * Represents a map texture.
 */
struct MapTexture {
    Point p0;           /// First point used when mapping the texture.
    Point p1;           /// Second point used when mapping the texture.
    PPMImage texture;   /// Texture data.

    /// Constructs a map texture.
    MapTexture(const char *filename) : texture(filename) { }
};

/**
 * Represents a material.
 */
struct Material {
    float ambientCoef;          /// Ambient coefficient.
    float diffuseCoef;          /// Diffuse coefficient.
    float specularCoef;         /// Specular coefficient.
    float specularExp;          /// Specular exponent.
    float reflectionCoef;       /// Reflection coefficient.
    float transmissionCoef;     /// Transmission coefficient;
    float refractionRate;       /// Refraction rate.
};

/**
 * Represents a sphere object.
 */
struct SphereObject {
    Point center;               /// Center of the sphere.
    float radius;               /// Radius of the sphere.
    TextureType textureType;    /// Texture type.
    int textureID;              /// Texture ID.
    int materialID;             /// Material ID.
};

/**
 * Represents a polyhedron object.
 */
struct PolyhedronObject {
    std::vector<Plane> faces;   /// Faces of the object.
    TextureType textureType;    /// Texture type of all the faces.
    int textureID;              /// ID of the texture of all the faces.
    int materialID;             /// ID of the material of all the faces.
};

/**
 * This class stores the world composition: objects, lights, textures, etc...
 * Everything here is constant through the entire execution of the raytracer.
 */
class World {
    /**
     * This struct stores the information that converts from the texture IDs at
     * the input file to the type and id used here.
     */
    struct TextureInfo {
        TextureType type;   /// Type of the texture.
        int id;             /// ID of the texture.
    };

    /// This vector converts from raw texture IDs to TextureInfo structs.
    std::vector<TextureInfo> _textureInfos;

    /// Jumps the camera description from the input (by jumping 4 lines).
    void ignoreCameraDescription(std::ifstream &in);

    /// Reads the light description from the input.
    void readLightDescription(std::ifstream &in);

    /// Reads the texture description from the input.
    void readTextureDescription(std::ifstream &in);

    /// Reads the material description from the input.
    void readMaterialDescription(std::ifstream &in);

    /// Reads the object description from the input.
    void readObjectDescription(std::ifstream &in);

public:
    /// Inits the world with the info from the input file.
    World(const char *aInput);

    std::vector<Light> lights;                          /// Light data.
    std::vector<SolidTexture> solidTextures;            /// Solid texture data.
    std::vector<CheckerTexture> checkerTextures;        /// Checker texture data.
    std::vector<MapTexture> mapTextures;                /// Map texture data.
    std::vector<Material> materials;                    /// Material data.
    std::vector<SphereObject> sphereObjects;            /// Sphere objects.
    std::vector<PolyhedronObject> polyhedronObjects;    /// Polyhedron objects.
};

#endif // !WORLD_HPP
