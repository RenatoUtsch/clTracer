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

#include "CodeGenerator.hpp"
#include "../error.hpp"
#include <sstream>

std::string CodeGenerator::generateStructures(const World &world) {
    return std::string(
        "typedef struct Light {\n"
        "    float4 pos;\n"
        "    float4 color;\n"
        "    float constantAtt;\n"
        "    float linearAtt;\n"
        "    float quadraticAtt;\n"
        "} Light;\n"
        "\n"
        "typedef enum TextureType {\n"
        "    SolidTextureType,\n"
        "    CheckerTextureType,\n"
        "    MapTextureType\n"
        "} TextureType;\n"
        "\n"
        "typedef struct SolidTexture {\n"
        "    float4 color;\n"
        "} SolidTexture;\n"
        "\n"
        "typedef struct CheckerTexture {\n"
        "    float4 color1;\n"
        "    float4 color2;\n"
        "    float size;\n"
        "} CheckerTexture;\n"
        "\n"
        "typedef struct MapTexture {\n"
        "    float4 p0;\n"
        "    float4 p1;\n"
        "    int width;\n"
        "    int height;\n"
        "    float4 *data;\n"
        "} MapTexture;\n"
        "\n"
        "typedef struct Material {\n"
        "    float ambientCoef;\n"
        "    float diffuseCoef;\n"
        "    float specularCoef;\n"
        "    float specularExp;\n"
        "    float reflectionCoef;\n"
        "    float transmissionCoef;\n"
        "    float refractionRate;\n"
        "} Material;\n"
        "\n"
        "typedef struct Sphere {\n"
        "    float4 center;\n"
        "    float radius;\n"
        "    TextureType textureType;\n"
        "    int textureID;\n"
        "    int materialID;\n"
        "} Sphere;\n"
        "\n"
        "typedef struct Polyhedron {\n"
        "    int textureID;\n"
        "    int materialID;\n"
        "    int numFaces;\n"
        "    TextureType textureType;\n"
        "    float4 faces[1];\n"
        "} Polyhedron;\n"
        "\n"
    );
}

std::string CodeGenerator::generateLights(const World &world) {
    std::stringstream code;

    stop_if(!world.lights.size(), "input needs at least one light.");

    code << "__constant int numLights = " << world.lights.size() << ";\n"
        << "__constant Light lights[] = {\n";

    for(size_t i = 0; i < world.lights.size(); ++i)
        code << "    " << writeLight(world.lights[i]) << ",\n";

    code << "};\n\n";

    return code.str();
}

std::string CodeGenerator::generateSolidTextures(const World &world) {
    std::stringstream code;

    if(world.solidTextures.size()) {
        code << "__constant SolidTexture solidTextures[] = {\n";

        for(size_t i = 0; i < world.solidTextures.size(); ++i)
            code << "    " << writeSolidTexture(world.solidTextures[i]) << ",\n";

        code << "};\n\n";
    }
    else {
        code << "__constant SolidTexture solidTextures[1]; // Dummy.\n";
    }

    return code.str();
}

std::string CodeGenerator::generateCheckerTextures(const World &world) {
    std::stringstream code;
/*
    if(world.checkerTextures.size()) {
        code << "__constant CheckerTexture checkerTextures[] = {\n";

        for(size_t i = 0; i < world.checkerTextures.size(); ++i)
            code << "    " << writeCheckerTexture(world.checkerTextures[i]) << ",\n";

        code << "};\n\n";
    }
    else {
        code << "__constant CheckerTexture checkerTextures[1]; // Dummy.\n";
    }
*/
    return code.str();
}

std::string CodeGenerator::generateMapTextures(const World &world) {
    std::stringstream code;

    if(!world.mapTextures.size()) return std::string();

    code << "__constant int numMapTextures = " << world.mapTextures.size()
        << ";\n"
        << "__constant MapTexture mapTextures[] = {\n";

    code << "};\n\n";

    return code.str();
}

std::string CodeGenerator::generateMaterials(const World &world) {
    std::stringstream code;

    stop_if(!world.materials.size(), "Input needs at least one material.");

    code << "__constant Material materials[] = {\n";

    for(size_t i = 0; i < world.materials.size(); ++i)
        code << "    " << writeMaterial(world.materials[i]) << ",\n";

    code << "};\n\n";

    return code.str();
}

std::string CodeGenerator::generateSpheres(const World &world) {
    std::stringstream code;

    code << "__constant int numSpheres = " << world.spheres.size() << ";\n";

    if(world.spheres.size()) {
        code << "__constant Sphere spheres[] = {\n";

        for(size_t i = 0; i < world.spheres.size(); ++i)
            code << "    " << writeSphere(world.spheres[i]) << ",\n";

        code << "};\n\n";
    }
    else {
        code << "__constant Sphere spheres[1]; // Dummy.\n";
    }

    return code.str();
}

std::string CodeGenerator::generatePolyhedrons(const World &world) {
    std::stringstream code;
/*
    code << "__constant int numPolyhedrons = " << world.polyhedrons.size()
        << ";\n";

    if(world.polyhedrons.size()) {
        code << "__constant Polyhedron polyhedrons[] = {\n";

        for(size_t i = 0; i < world.polyhedrons.size(); ++i)
            code << "    " << writePolyhedron(world.polyhedrons[i]) << ",\n";

        code << "};\n\n";
    }
    else {
        code << "__constant Polyhedron polyhedrons[1]; // Dummy.\n";
    }
*/
    return code.str();
}

std::string CodeGenerator::writeLight(const Light &light) {
    std::stringstream code;

    code << "{ "
        << writePoint(light.pos) << ", "
        << writeColor(light.color)
        << ", (float) " << light.constantAtt
        << ", (float) " << light.linearAtt
        << ", (float) " << light.quadraticAtt
        << " }";

    return code.str();
}

std::string CodeGenerator::writeSolidTexture(const SolidTexture &tex) {
    std::stringstream code;

    code << "{ " << writeColor(tex.color) << " }";

    return code.str();
}

std::string CodeGenerator::writeCheckerTexture(const CheckerTexture &tex) {
    std::stringstream code;

    code << "{ "
        << writeColor(tex.color1) << ", "
        << writeColor(tex.color2) << ", "
        << "(float) " << tex.size
        <<  " }";

    return code.str();
}

std::string CodeGenerator::writeMaterial(const Material &material) {
    std::stringstream code;

    code << "{ (float) " << material.ambientCoef
        << ", (float) " << material.diffuseCoef
        << ", (float) " << material.specularCoef
        << ", (float) " << material.specularExp
        << ", (float) " << material.reflectionCoef
        << ", (float) " << material.transmissionCoef
        << ", (float) " << material.refractionRate
        << " }";

    return code.str();
}

std::string CodeGenerator::writeSphere(const Sphere &sphere) {
    std::stringstream code;

    code << "{ "
        << writePoint(sphere.center) << ", "
        << "(float) " << sphere.radius << ", "
        << "(TextureType) " << sphere.textureType << ", "
        << "(int) " << sphere.textureID << ", "
        << "(int) " << sphere.materialID << ", "
        << " }";

    return code.str();
}

std::string CodeGenerator::writePolyhedron(const Polyhedron &polyhedron) {
    std::stringstream code;

    code << "{ "
        << "(int) " << polyhedron.textureID << ", "
        << "(int) " << polyhedron.materialID << ", "
        << "(int) " << polyhedron.faces.size() << ", "
        << "(TextureType) " << polyhedron.textureType << ", "
        << "{ ";

    for(size_t i = 0; i < polyhedron.faces.size(); ++i) {
        code << writePlane(polyhedron.faces[i]) << ", ";
    }

    code << "}"
        << " }";

    return code.str();
}

std::string CodeGenerator::writePoint(const Point &point) {
    std::stringstream code;
    code << "(float4) (" << point.x << ", " << point.y << ", " << point.z
        << ", " << point.w << ")";

    return code.str();
}

std::string CodeGenerator::writePlane(const Plane &plane) {
    std::stringstream code;
    code << "(float4) (" << plane.a << ", " << plane.b << ", " << plane.c
        << ", " << plane.d << ")";

    return code.str();
}

std::string CodeGenerator::writeColor(const Color &color) {
    std::stringstream code;
    code << "(float4) (" << color.r << ", " << color.g << ", " << color.b
        << ", 1.0f)";

    return code.str();
}

std::string CodeGenerator::generateCode(const World &world) {
    std::stringstream code;

    code << "// Generated code. Do not change, as these changes will be lost.\n\n";

    code << generateStructures(world)
        << generateLights(world)
        << generateSolidTextures(world)
        << generateCheckerTextures(world)
        << generateMapTextures(world)
        << generateMaterials(world)
        << generateSpheres(world)
        << generatePolyhedrons(world);

    return code.str();
}
