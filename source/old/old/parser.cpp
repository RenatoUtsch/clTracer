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

#include "parser.hpp"
#include "error.hpp"
#include "types.hpp"
#include "scene.hpp"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

static void calculateScreen(Scene &scene) {
    // If the distance from the camera to the center of the screen is d and
    // the fovy is f, the maximum height of the screen is:
    // A schematic (from the side);
    //                            .
    //                         .  |
    //                     .      | <- h
    //                  .         |
    //               .  f         |
    // Camera -> .----------------. <- Screen center
    //               .     ^d     |
    //                  .         |
    //                     .      | <- h
    //                         .  |
    //                            .
    // tan(f) = h/d => h = tan(f) * d;
    // The height of the screen is h = tan(f) * d.
    //
    // The we know that if the screen has n pixels from top to low (2h), then
    // 2h ~ n
    // The width 2w of the screen, with m pixels, is:
    // 2w ~ m
    // Then 2w = (2h*m)/n
    float d = Point::distance(scene.screen.center, scene.camera.pos);

    // Sizes
    scene.screen.height = 2 * tan(toRads(scene.camera.fovy / 2.0f)) * d;
    scene.screen.width = (scene.screen.height * scene.screen.widthNumPixels)
        / scene.screen.heightNumPixels;

    // Directions
    scene.screen.rightDir = Vector::cross(scene.camera.dir, scene.camera.up);
    scene.screen.rightDir.normalize();
    scene.screen.bottomDir = scene.camera.up * (-1);
    scene.screen.bottomDir.normalize();

    // Top left pixel.
    scene.screen.topLeft = scene.screen.center;
    scene.screen.topLeft -= scene.screen.rightDir * (scene.screen.width / 2.0f);
    scene.screen.topLeft += scene.camera.up * (scene.screen.height / 2.0f);

    // Pixel size.
    scene.screen.pixelWidth = scene.screen.width / (float) scene.screen.widthNumPixels;
    scene.screen.pixelHeight = scene.screen.height / (float) scene.screen.heightNumPixels;
}

static void readCameraDescription(std::ifstream &in, Scene &scene) {
    in >> scene.camera.pos.x >> scene.camera.pos.y >> scene.camera.pos.z;

    in >> scene.screen.center.x >> scene.screen.center.y >> scene.screen.center.z;
    scene.camera.dir = scene.screen.center - scene.camera.pos;
    scene.camera.dir.normalize();

    in >> scene.camera.up.x >> scene.camera.up.y >> scene.camera.up.z;
    scene.camera.up.normalize();
    // Make up orthogonal to dir.
    scene.camera.up -= Vector::proj(scene.camera.dir, scene.camera.up);
    scene.camera.up.normalize();

    in >> scene.camera.fovy;

    // Calculate the screen now.
    calculateScreen(scene);
}

static void readLightDescription(std::ifstream &in, Scene &scene) {
    Light light;

    in >> scene.numLights;
    if(in.fail())
        fatalError("Failed to read the input file(material description)");

    // Read the ambient light.
    in >> scene.ambient.pos.x >> scene.ambient.pos.y >> scene.ambient.pos.z;
    in >> scene.ambient.color.r >> scene.ambient.color.g >> scene.ambient.color.b;
    in >> scene.ambient.attenuation.constant >> scene.ambient.attenuation.linear
        >> scene.ambient.attenuation.quadratic;

    // Read the other lights.
    for(size_t i = 1; i < scene.numLights; ++i) {
        in >> light.pos.x >> light.pos.y >> light.pos.z;
        in >> light.color.r >> light.color.g >> light.color.b;
        in >> light.attenuation.constant >> light.attenuation.linear
            >> light.attenuation.quadratic;
        scene.lights.push_back(light);
    }
}

static void readTextureDescription(std::ifstream &in, Scene &scene) {
    std::string type;
    Texture tex;

    in >> scene.numTextures;
    if(in.fail())
        fatalError("Failed to read the input file (material description)");

    for(Texture::Id i = 0; i < scene.numTextures; ++i) {
        tex.id = i;
        in >> type;

        if(type == "solid") {
            tex.type = SolidTextureType;
            in >> tex.solid.color.r >> tex.solid.color.g >> tex.solid.color.b;
        }
        else if(type == "checker") {
            tex.type = CheckerTextureType;
            in >> tex.checker.color1.r >> tex.checker.color1.g
                >> tex.checker.color1.b;
            in >> tex.checker.color2.r >> tex.checker.color2.g
                >> tex.checker.color2.b;
            in >> tex.checker.size;

            tex.checker.map[0][0] = tex.checker.color1;
            tex.checker.map[0][1] = tex.checker.color2;
            tex.checker.map[1][0] = tex.checker.color1;
            tex.checker.map[1][1] = tex.checker.color2;
        }
        else if(type == "texmap") {
            tex.type = MapTextureType;
            in >> tex.map.filename;
            in >> tex.map.p0.x >> tex.map.p0.y >> tex.map.p0.z >> tex.map.p0.w;
            in >> tex.map.p1.x >> tex.map.p1.y >> tex.map.p1.z >> tex.map.p1.w;
            tex.map.ppm.load(tex.map.filename);
        }
        else {
            fatalError("invalid texture (%s)", type.c_str());
        }

        scene.textures.push_back(tex);
    }
}

void readMaterialDescription(std::istream &in, Scene &scene) {
    Material material;

    in >> scene.numMaterials;
    if(in.fail())
        fatalError("Failed to read the input file (material description)");

    for(Material::Id i = 0; i < scene.numMaterials; ++i) {
        material.id = i;
        in >> material.ambientCoef >> material.diffuseCoef
            >> material.specularCoef >> material.specularExp
            >> material.reflectionCoef >> material.transmissionCoef
            >> material.refractionRate;
        scene.materials.push_back(material);
    }
}


void readObjectDescription(std::istream &in, Scene &scene) {
    Object object;
    std::string type;

    in >> scene.numObjects;
    if(in.fail())
        fatalError("Failed to read the input file (object description)");

    for(Object::Id i = 0; i < scene.numObjects; ++i) {
        object.id = i;
        object.scene = &scene;
        in >> object.textureId >> object.materialId;
        in >> type;

        if(type == "sphere") {
            object.type = SphereObjectType;
            in >> object.sphere.pos.x >> object.sphere.pos.y
                >> object.sphere.pos.z;
            in >> object.sphere.radius;

        }
        else if(type == "polyhedron") {
            object.type = PolyhedronObjectType;
            in >> object.polyhedron.numFaces;
            if(in.fail())
                fatalError("Failed to read the input file (polyhedron description)");

            Plane plane;
            for(size_t j = 0; j < object.polyhedron.numFaces; ++j) {
                in >> plane.a >> plane.b >> plane.c >> plane.d;
                object.polyhedron.planes.push_back(plane);
            }
        }
        else {
            fatalError("invalid object (%s)", type.c_str());
        }

        scene.objects.push_back(object);

        // Clear the temporary object.
        object.polyhedron.planes.clear();
    }
}

void parseArgs(int argc, char **argv, Scene &scene) {
    // Check the number of arguments.
    if(argc == 3) {
        scene.input = argv[1];
        scene.output = argv[2];
        scene.screen.widthNumPixels = 800;
        scene.screen.heightNumPixels = 600;
    }
    else if(argc == 5) {
        scene.input = argv[1];
        scene.output = argv[2];
        scene.screen.widthNumPixels = strtoul(argv[3], NULL, 10);
        scene.screen.heightNumPixels = strtoul(argv[4], NULL, 10);
    }
    else {
        std::cerr << "Usage: " << argv[0] << " input output [width] [height]" << std::endl;
        exit(1);
    }

    // Open the input file.
    std::ifstream input(scene.input);
    if(!input.is_open())
        fatalError("Failed to open input file (%s)", scene.input);

    // Read the input.
    readCameraDescription(input, scene);
    readLightDescription(input, scene);
    readTextureDescription(input, scene);
    readMaterialDescription(input, scene);
    readObjectDescription(input, scene);

    // Close the input file.
    input.close();
}
