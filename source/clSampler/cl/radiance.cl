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

#ifndef RADIANCE_CL
#define RADIANCE_CL

#include "intersection.cl"
#include "object.cl"
#include "random.cl"
#include "recursion.cl"
#include "brdf.cl"

/**
 * Calculates the color of the ray.
 * @param origin Ray origin.
 * @param dir Ray direction.
 * @param seed Random seed.
 * @return Color that was sampled.
 */
float4 radiance(float4 *origin, float4 *dir, uint2 *seed);

/**
 * Stages of the radiance recursion.
 */
void radianceStage0(Stack *stack, RetStack *retStack, State *t, uint2 *seed);
void radianceStage1(Stack *stack, RetStack *retStack, State *t, uint2 *seed);

float4 radiance(float4 *argOrigin, float4 *argDir, uint2 *seed) {
    Stack stack; // Recursion stack.
    RetStack retStack; // Return stack.
    State *t; // Top state.

    stackInit(&stack);
    retStackInit(&retStack);

    t = stackTop(&stack);
    initState(t, *argOrigin, *argDir, NoIntersection, -1);
    stackPush(&stack);

    // Simulated recursion.
    while(!stackEmpty(&stack)) {
        stackPop(&stack);
        t = stackTop(&stack);
        switch(t->stage) {
            case 0: radianceStage0(&stack, &retStack, t, seed); break;
            case 1: radianceStage1(&stack, &retStack, t, seed); break;
        }
    }

    // Return the top of the stack.
    retStackPop(&retStack);
    return *retStackTop(&retStack);
}

void radianceStage0(Stack *stack, RetStack *retStack, State *t, uint2 *seed) {
    float4 intersection, normal;
    IntersectionType iType;
    int id;
    bool inside;

    // See if the ray intersects anything.
    iType = trace(t->origin, t->dir, t->exclType, t->exclID, 0, &id,
            &intersection, &normal, &inside);

    if(iType == NoIntersection) { // Don't need to do anything anymore.
        float4 *r = retStackTop(retStack);
        *r = (float4) (0.0f, 0.0f, 0.0f, 1.0f);
        retStackPush(retStack);
        return;
    }

    // If is emitter, return the emitted color.
    if(iType == SphereIntersection && sphereEmits(id)) {
        float4 *r = retStackTop(retStack);
        *r = spheres[id].emission;
        retStackPush(retStack);
        return;
    }

    // Russian roulette.
    float u = randf(seed);
    float rr = clamp(dot(-1.0f * t->dir, normal), 0.0f, 1.0f);
    if(u < rr) { // Trace ray.
        float4 newDir, color, f;
        float pdf;
        int matID, texID;
        TextureType texType;

        getObjectIDs(iType, id, &matID, &texType, &texID);
        color = getTextureColor(texType, texID, intersection);
        if(brdf(t->dir, normal, &materials[matID], inside, seed, &newDir, &f, &pdf)) {
            t->factor = color * f / (pdf * rr);

            t->stage = 1;
            stackPush(stack); // Set this for when the recursion returns.

            // Push new recursion.
            State *newT = stackTop(stack);
            initState(newT, intersection, newDir, iType, id);
            stackPush(stack); // New iteration.
        }
        else {
            float4 *r = retStackTop(retStack);
            *r = (float4) (0.0f, 0.0f, 0.0f, 1.0f);
            retStackPush(retStack);
        }
    }
    else { // Return no contribution.
        float4 *r = retStackTop(retStack);
        *r = (float4) (0.0f, 0.0f, 0.0f, 1.0f);
        retStackPush(retStack);
    }
}

void radianceStage1(Stack *stack, RetStack *retStack, State *t, uint2 *seed) {
    retStackPop(retStack);
    float4 *r = retStackTop(retStack);
    *r *= t->factor; // Calculate the proper light.
    retStackPush(retStack); // Return.
}

#endif // !RADIANCE_CL
