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

#include "parser.h"
#include "error.h"
#include "types.h"
#include "vecmath.h"
#include <stdio.h>
#include <stdlib.h>

static void calculateScreenCoordinates(Scene *scene) {
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
    float d = pointDistance(scene->screen.center, scene->camera.pos);

    // Calculate the sizes.
    scene->screen.height = 2 * tan(toRads(scene->camera.fovy / 2.0f)) * d;
    scene->screen.width = (scene->screen.height * scene->screen.widthInPixels)
        / scene->screen.widthInPixels;

    // Directions.
    vecCross(scene->camera.dir, scene->camera.up, scene->screen.rightDir);
    vecNormalize(scene->screen.rightDir);
    vecScale(scene->camera.up, -1.0f, scene->screen.bottomDir);
    vecNormalize(scene->screen.bottomDir);

    // Top left pixel.
    Vector vec;
    pointAssign(scene->screen.center, scene->screen.topLeftPixel);
    vecScale(scene->screen.rightDir, -1.0f * scene->screen.width / 2.0f, vec);
    pointVecAdd(scene->screen.topLeftPixel, vec, scene->screen.topLeftPixel);
    vecScale(scene->camera.up, scene->screen.height / 2.0f, vec);
    pointVecAdd(scene->screen.topLeftPixel, vec, scene->screen.topLeftPixel);

    // Pixel size.
    scene->screen.pixelWidth = scene->screen.width
        / (float) scene->screen.widthInPixels;
    scene->screen.pixelHeight = scene->screen.height
        / (float) scene->screen.heightInPixels;
}

static void readCameraDescription(FILE *input, Scene *scene) {
    fscanf(input, "%f%f%f", &scene->camera.pos[0], &scene->camera.pos[1],
            &scene->camera.pos[2]);
    scene->camera.pos[3] = 1.0f;

    fscanf(input, "%f%f%f", &scene->screen.center[0], &scene->screen.center[1],
            &scene->screen.center[2]);
    scene->screen.center[3] = 1.0f;

    pointSub(scene->screen.center, scene->camera.pos, scene->camera.dir);
    vecNormalize(scene->camera.dir);

    fscanf(input, "%f%f%f", &scene->camera.up[0], &scene->camera.up[1],
            &scene->camera.up[2]);
    scene->camera.up[3] = 0.0f;

    // Make up orthogonal to dir.
    Vector proj;
    vecProj(scene->camera.dir, scene->camera.up, proj);
    vecSub(scene->camera.up, proj, scene->camera.up);
    vecNormalize(scene->camera.up);

    fscanf(input, "%f", &scene->camera.fovy);

    calculateScreenCoordinates(scene);
}

static void readLightDescription(FILE *input, Scene *scene) {

}

static void readTextureDescription(FILE *input, Scene *scene) {

}

static void readMaterialDescription(FILE *input, Scene *scene) {

}

static void readObjectDescription(FILE *input, Scene *scene) {

}

Scene *parseInput(int argc, char **argv) {
    if(argc != 3 && argc != 5) {
        fprintf(stderr, "Usage: %s input output [width] [height]\n", argv[0]);
        exit(1);
    }

    Scene *scene = malloc(sizeof(*scene));
    stop_if(!scene, "failed to allocate the scene structure.");

    scene->input = argv[1];
    scene->output = argv[2];

    if(argc == 5) {
        scene->screen.widthInPixels = (int) strtol(argv[3], NULL, 10);
        scene->screen.heightInPixels = (int) strtol(argv[4], NULL, 10);
    }
    else {
        scene->screen.widthInPixels = 800;
        scene->screen.heightInPixels = 600;
    }

    fflush(stderr);
    FILE *input = fopen(scene->input, "r");
    stop_if(!input, "failed to open input file.");

    // Read input.
    readCameraDescription(input, scene);
    readLightDescription(input, scene);
    readTextureDescription(input, scene);
    readMaterialDescription(input, scene);
    readObjectDescription(input, scene);

    fclose(input);
    input = NULL;

    return scene;
}
