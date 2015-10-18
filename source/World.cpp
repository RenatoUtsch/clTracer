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

#include "World.hpp"
#include "error.hpp"
#include <fstream>
#include <string>

World::World(const std::string &aInput) {
    std::ifstream in(aInput.c_str());
    stop_if(!in.is_open(), "failed to open input file.");

    ignoreCameraDescription(in);
    readLightDescription(in);
    readTextureDescription(in, aInput.substr(0, aInput.rfind('/') + 1));
    readMaterialDescription(in);
    readObjectDescription(in);
}

void World::ignoreCameraDescription(std::ifstream &in) {
    // The camera description has 4 lines.
    in.ignore(std::numeric_limits<std::streamsize>::max(), in.widen('\n'));
    in.ignore(std::numeric_limits<std::streamsize>::max(), in.widen('\n'));
    in.ignore(std::numeric_limits<std::streamsize>::max(), in.widen('\n'));
    in.ignore(std::numeric_limits<std::streamsize>::max(), in.widen('\n'));
}

void World::readLightDescription(std::ifstream &in) {
    Light light;
    int numLights;

    in >> numLights;
    for(int i = 0; i < numLights; ++i) {
        in >> light.pos.x >> light.pos.y >> light.pos.z;
        in >> light.color.r >> light.color.g >> light.color.b;
        in >> light.constantAtt >> light.linearAtt >> light.quadraticAtt;

        lights.push_back(light);
    }
}

void World::readTextureDescription(std::ifstream &in, const std::string &inputPath) {
    TextureInfo info;
    std::string type;
    int numTextures;
    int numSolidTextures = 0, numCheckerTextures = 0, numMapTextures = 0;

    in >> numTextures;
    for(int i = 0; i < numTextures; ++i) {
        in >> type;

        if(type == "solid") {
            info.type = SolidTextureType;
            info.id = numSolidTextures++;
            _textureInfos.push_back(info);

            SolidTexture solid;
            in >> solid.color.r >> solid.color.g >> solid.color.b;

            solidTextures.push_back(solid);
        }
        else if(type == "checker") {
            info.type = CheckerTextureType;
            info.id = numCheckerTextures++;
            _textureInfos.push_back(info);

            CheckerTexture checker;
            in >> checker.color1.r >> checker.color1.g >> checker.color1.b;
            in >> checker.color2.r >> checker.color2.g >> checker.color2.b;
            in >> checker.size;

            checkerTextures.push_back(checker);
        }
        else if(type == "texmap") {
            info.type = MapTextureType;
            info.id = numMapTextures++;
            _textureInfos.push_back(info);

            std::string filename;
            in >> filename;

            MapTexture map((inputPath + filename).c_str());
            in >> map.p0.x >> map.p0.y >> map.p0.z >> map.p0.w;
            in >> map.p1.x >> map.p1.y >> map.p1.z >> map.p1.w;

            mapTextures.push_back(map);
        }
        else {
            stop_if(true, "invalide texture (%s).", type.c_str());
        }
    }
}

void World::readMaterialDescription(std::ifstream &in) {
    Material material;
    int numMaterials;

    in >> numMaterials;
    for(int i = 0; i < numMaterials; ++i) {
        in >> material.ambientCoef >> material.diffuseCoef
            >> material.specularCoef >> material.specularExp
            >> material.reflectionCoef >> material.transmissionCoef
            >> material.refractionRate;

        materials.push_back(material);
    }
}

void World::readObjectDescription(std::ifstream &in) {
    std::string type;
    int numObjects, rawTextureID, materialID;

    in >> numObjects;
    for(int i = 0; i < numObjects; ++i) {
        in >> rawTextureID >> materialID;
        in >> type;

        if(type == "sphere") {
            Sphere obj;
            obj.textureType = _textureInfos[rawTextureID].type;
            obj.textureID = _textureInfos[rawTextureID].id;
            obj.materialID = materialID;

            in >> obj.center.x >> obj.center.y >> obj.center.z >> obj.radius;

            spheres.push_back(obj);
        }
        else if(type == "polyhedron") {
            Polyhedron obj;
            Plane plane;
            int numFaces;

            obj.textureType = _textureInfos[rawTextureID].type;
            obj.textureID = _textureInfos[rawTextureID].id;
            obj.materialID = materialID;

            in >> numFaces;
            for(int j = 0; j < numFaces; ++j) {
                in >> plane.a >> plane.b >> plane.c >> plane.d;
                obj.faces.push_back(plane);
            }

            polyhedrons.push_back(obj);
        }
        else {
            stop_if(true, "invalid object (%s).", type.c_str());
        }
    }
}
