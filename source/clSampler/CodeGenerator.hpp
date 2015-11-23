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

#ifndef CLSAMPLER_CODEGENERATOR_HPP
#define CLSAMPLER_CODEGENERATOR_HPP

#include "../World.hpp"
#include "../Screen.hpp"
#include "../CmdArgs.hpp"
#include <string>

/**
 * Generates OpenCL code that represents the given World.
 */
class CodeGenerator {
    /// Generates the structures.
    std::string generateStructures(const World &world);

    /// Generates the constants.
    std::string generateConstants(const Screen &screen, const CmdArgs &args);

    /// Generates the solid textures.
    std::string generateSolidTextures(const World &world);

    /// Generates the checker textures.
    std::string generateCheckerTextures(const World &world);

    /// Generates the map textures.
    std::string generateMapTextures(const World &world);

    /// Generates the materials.
    std::string generateMaterials(const World &world);

    /// Generates the sphere objects.
    std::string generateSpheres(const World &world);

    /// Generates the polyhedron objects.
    std::string generatePolyhedrons(const World &world);

    /// Writes a solid texture.
    std::string writeSolidTexture(const SolidTexture &tex);

    /// Writes a checker texture.
    std::string writeCheckerTexture(const CheckerTexture &tex);

    /// Writes a map texture.
    std::string writeMapTexture(const MapTexture &tex, int texIndex);

    /// Writes a material.
    std::string writeMaterial(const Material &material);

    /// Writes a sphere object.
    std::string writeSphere(const Sphere &sphere);

    /// Writes a polyhedron object.
    std::string writePolyhedron(const Polyhedron &polyhedron, int faceIndex);

    /// Writes a point structure.
    std::string writePoint(const Point &point);

    /// Writes a plane structure.
    std::string writePlane(const Plane &plane);

    /// Writes a color structure.
    std::string writeColor(const Color &color);

    /// Writes a float.
    std::string writeFloat(float val);

public:
    /// Generates code about the given world and returns it.
    std::string generateCode(const World &world, const Screen &screen,
            const CmdArgs &args);
};

#endif // !CLSAMPLER_CODEGENERATOR_HPP
