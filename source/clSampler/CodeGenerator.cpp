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
        "    int dataBegin;\n"
        "    int width;\n"
        "    int height;\n"
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
        "    float radius2;\n"
        "    TextureType textureType;\n"
        "    int textureID;\n"
        "    int materialID;\n"
        "} Sphere;\n"
        "\n"
        "typedef struct Polyhedron {\n"
        "    int numFaces;\n"
        "    int facesBegin;\n"
        "    TextureType textureType;\n"
        "    int textureID;\n"
        "    int materialID;\n"
        "} Polyhedron;\n"
        "\n"
    );
}

std::string CodeGenerator::generateLights(const World &world) {
    std::stringstream code;

    stop_if(!world.lights.size(), "input needs at least one light.");

    code << "#define numLights " << world.lights.size() << "\n\n"
        << "__constant Light lights[] = {\n";

    for(size_t i = 0; i < world.lights.size(); ++i) {
        code << "    " << writeLight(world.lights[i]);
        if(i != world.lights.size() - 1)
            code << ",";
        code << "\n";
    }

    code << "};\n\n";

    return code.str();
}

std::string CodeGenerator::generateSolidTextures(const World &world) {
    std::stringstream code;

    code << "#define numSolidTextures " << world.solidTextures.size() << "\n\n";

    if(world.solidTextures.size()) {
        code << "__constant SolidTexture solidTextures[] = {\n";

        for(size_t i = 0; i < world.solidTextures.size(); ++i) {
            code << "    " << writeSolidTexture(world.solidTextures[i]);
            if(i != world.solidTextures.size() - 1)
                code << ",";
            code << "\n";
        }

        code << "};\n\n";
    }
    else {
        code << "__constant SolidTexture solidTextures[1]; // Dummy.\n\n";
    }

    return code.str();
}

std::string CodeGenerator::generateCheckerTextures(const World &world) {
    std::stringstream code;

    code << "#define numCheckerTextures " << world.checkerTextures.size()
        << "\n\n";

    if(world.checkerTextures.size()) {
        code << "__constant CheckerTexture checkerTextures[] = {\n";

        for(size_t i = 0; i < world.checkerTextures.size(); ++i) {
            code << "    " << writeCheckerTexture(world.checkerTextures[i]);
            if(i != world.checkerTextures.size() - 1)
                code << ",";
            code << "\n";
        }

        code << "};\n\n";
    }
    else {
        code << "__constant CheckerTexture checkerTextures[1]; // Dummy.\n\n";
    }

    return code.str();
}

std::string CodeGenerator::generateMapTextures(const World &world) {
    std::stringstream code;
    int texIndex = 0;

    code << "#define numMapTextures " << world.mapTextures.size() << "\n\n";

    if(world.mapTextures.size()) {
        code << "__constant MapTexture mapTextures[] = {\n";

        for(size_t i = 0; i < world.mapTextures.size(); ++i) {
            code << "    " << writeMapTexture(world.mapTextures[i], texIndex);
            if(i != world.mapTextures.size() - 1)
                code << ",";
            code << "\n";

            texIndex += world.mapTextures[i].texture.width()
                * world.mapTextures[i].texture.height();
        }

        code << "};\n\n";

        code << "__constant float4 mapData[] = {\n";

        for(size_t i = 0; i < world.mapTextures.size(); ++i) {
            const auto &data = world.mapTextures[i].texture.data;
            for(size_t j = 0; j < data.size(); ++j) {
                for(size_t k = 0; k < data[j].size(); ++k) {
                    code << "{ "
                        << writeColor(data[j].at(k))
                        << " }";
                    if(k + 1 != data[j].size())
                        code << ", ";
                }
                if(j + 1 != data[j].size())
                    code << ",";
                code << "\n";
            }
        }

        code << "};\n\n";
    }
    else {
        code << "__constant MapTexture mapTextures[1]; // Dummy.\n\n";
        code << "__constant float4 mapData[1]; // Dummy.\n\n";
    }

    return code.str();
}

std::string CodeGenerator::generateMaterials(const World &world) {
    std::stringstream code;

    stop_if(!world.materials.size(), "Input needs at least one material.");

    code << "#define numMaterials " << world.materials.size() << "\n\n"
        << "__constant Material materials[] = {\n";

    for(size_t i = 0; i < world.materials.size(); ++i) {
        code << "    " << writeMaterial(world.materials[i]);
        if(i != world.materials.size() - 1)
            code << ",";
        code << "\n";
    }

    code << "};\n\n";

    return code.str();
}

std::string CodeGenerator::generateSpheres(const World &world) {
    std::stringstream code;

    code << "#define numSpheres " << world.spheres.size() << "\n\n";

    if(world.spheres.size()) {
        code << "__constant Sphere spheres[] = {\n";

        for(size_t i = 0; i < world.spheres.size(); ++i) {
            code << "    " << writeSphere(world.spheres[i]);
            if(i != world.spheres.size() - 1)
                code << ",";
            code << "\n";
        }

        code << "};\n\n";
    }
    else {
        code << "__constant Sphere spheres[1]; // Dummy.\n\n";
    }

    return code.str();
}

std::string CodeGenerator::generatePolyhedrons(const World &world) {
    std::stringstream code;
    size_t faceIndex = 0;

    code << "#define numPolyhedrons " << world.polyhedrons.size() << "\n\n";

    if(world.polyhedrons.size()) {
        code << "__constant float4 polyhedronFaces[] = {\n";

        size_t k = 0;
        for(size_t i = 0; i < world.polyhedrons.size(); ++i) {
            for(size_t j = 0; j < world.polyhedrons[i].faces.size(); ++j) {
                code << "    " << writePlane(world.polyhedrons[i].faces[j]);
                //if(k + 1 != faceIndex)
                    code << ",";
                code << "\n";
            }
            k += world.polyhedrons[i].faces.size();
        }

        code << "};\n\n";

        code << "__constant Polyhedron polyhedrons[] = {\n";
        for(size_t i = 0; i < world.polyhedrons.size(); ++i) {
            code << "    " << writePolyhedron(world.polyhedrons[i], faceIndex);
            if(i != world.polyhedrons.size() - 1)
                code << ",";
            code << "\n";
            faceIndex += world.polyhedrons[i].faces.size();
        }
        code << "};\n\n";
    }
    else {
        code << "__constant Polyhedron polyhedrons[1]; // Dummy.\n\n";
        code << "__constant float4 polyhedronFaces[1]; // Dummy.\n\n";
    }

    return code.str();
}

std::string CodeGenerator::writeLight(const Light &light) {
    std::stringstream code;

    code << "{ "
        << writePoint(light.pos) << ", "
        << writeColor(light.color) << ", "
        << writeFloat(light.constantAtt) << ", "
        << writeFloat(light.linearAtt) << ", "
        << writeFloat(light.quadraticAtt)
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
        << writeFloat(tex.size)
        <<  " }";

    return code.str();
}

std::string CodeGenerator::writeMapTexture(const MapTexture &tex, int texIndex) {
    std::stringstream code;

    code << "{ "
        << writePoint(tex.p0) << ", "
        << writePoint(tex.p1) << ", "
        << texIndex << ", "
        << tex.texture.width() << ", "
        << tex.texture.height()
        << " }";

    return code.str();
}

std::string CodeGenerator::writeMaterial(const Material &material) {
    std::stringstream code;

    code << "{ "
        << writeFloat(material.ambientCoef) << ", "
        << writeFloat(material.diffuseCoef) << ", "
        << writeFloat(material.specularCoef) << ", "
        << writeFloat(material.specularExp) << ", "
        << writeFloat(material.reflectionCoef) << ", "
        << writeFloat(material.transmissionCoef) << ", "
        << writeFloat(material.refractionRate)
        << " }";

    return code.str();
}

std::string CodeGenerator::writeSphere(const Sphere &sphere) {
    std::stringstream code;

    code << "{ "
        << writePoint(sphere.center) << ", "
        << writeFloat(sphere.radius2) << ", "
        << sphere.textureType << ", "
        << sphere.textureID << ", "
        << sphere.materialID
        << " }";

    return code.str();
}

std::string CodeGenerator::writePolyhedron(const Polyhedron &polyhedron,
        int faceIndex) {
    std::stringstream code;

    code << "{ "
        << polyhedron.faces.size() << ", "
        << faceIndex << ", "
        << polyhedron.textureType << ", "
        << polyhedron.textureID << ", "
        << polyhedron.materialID
        << " }";

    return code.str();
}

std::string CodeGenerator::writePoint(const Point &point) {
    std::stringstream code;
    code << "(float4) ("
        << writeFloat(point.x) << ", "
        << writeFloat(point.y) << ", "
        << writeFloat(point.z)
        << ", 1.0f)";

    return code.str();
}

std::string CodeGenerator::writePlane(const Plane &plane) {
    std::stringstream code;
    code << "(float4) ("
        << writeFloat(plane.a) << ", "
        << writeFloat(plane.b) << ", "
        << writeFloat(plane.c) << ", "
        << writeFloat(plane.d)
        << ")";

    return code.str();
}

std::string CodeGenerator::writeColor(const Color &color) {
    std::stringstream code;
    code << "(float4) ("
        << writeFloat(color.r) << ", "
        << writeFloat(color.g) << ", "
        << writeFloat(color.b)
        << ", 1.0f)";

    return code.str();
}

std::string CodeGenerator::writeFloat(float val) {
    std::stringstream code;

    code << val;

    if(code.str().find('.') == std::string::npos) {
        code << ".0";
    }
    if(code.str().find('f') == std::string::npos) {
        code << "f";
    }

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
