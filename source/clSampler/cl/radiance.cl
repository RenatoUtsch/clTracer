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
#include "direction.cl"
#include "object.cl"
#include "random.cl"
#include "recursion.cl"

/**
 * Calculates the local color component.
 * @param eyeDir Direction from the eye (camera).
 * @param iType Type of the first intersection.
 * @param id ID of the object of the first intersection.
 * @param intersection Coordinates of intersection.
 * @param normal Normal at intersection point.
 * @param material Material of the intersected object.
 * @param seed Random seed.
 * @param ambDiffColor output ambient + diffuse color.
 * @param specularCOlor output specular color.
 */
void calculateLocalColor(float4 eyeDir, IntersectionType iType, int id,
        float4 intersection, float4 normal, __constant Material *material,
        uint2 *seed, float4 *ambDiffColor, float4 *specularColor);

/**
 * Calculates the final color of the state.
 */
float4 calculateFinalColor(State *t);


/**
 * Stages of the radiance recursion.
 */
void radianceStage0(Stack *stack, RetStack *retStack, State *t);
void radianceStage1(Stack *stack, RetStack *retStack, State *t);
void radianceStage2(RetStack *retStack, State *t);
void radianceStage3(RetStack *retStack, State *t);

/**
 * Calculates the color of the ray.
 * @param origin Ray origin.
 * @param dir Ray direction.
 * @param seed Random seed.
 * @return Color that was sampled.
 */
float4 radiance(float4 *origin, float4 *dir, uint2 *seed);

void calculateLocalColor(float4 eyeDir, IntersectionType iType, int id,
        float4 intersection, float4 normal, __constant Material *material,
        uint2 *seed, float4 *ambDiffColor, float4 *specularColor)
{
    *ambDiffColor = ambientLight.color * material->ambientCoef; // Add ambient.
    *specularColor = (float4) (0.0f);

    // For the N light samples, choose a random light and direction to the
    // light and calculate the local light component.
    float4 diffLight = (float4) (0.0f);
    float4 specLight = (float4) (0.0f);
    for(int i = 0; i < NumLightSamples; ++i) {
        // Get a random light and light position.
        uint lightID = randn(NumLights, seed);
        float4 lightPos = getRandomLightPos(&intersection, lightID, seed);
        float4 lightDir = normalize(lightPos - intersection);
        float dist = distance(lightPos, intersection);

        // Calculate the light attenuation.
        float att = 1.0f / (lights[lightID].constantAtt
                + dist * lights[lightID].linearAtt
                + pow(dist, 2) * lights[lightID].quadraticAtt);

        // If the feeler ray does not intersect the surface of any object before
        // reaching the light, then the object is not shadowed.
        // If the object intersects, then just use the ambient color and ignore
        // the others.
        if(trace(intersection, lightDir, iType, id, &lightPos,
                    0, 0, 0, 0) == NoIntersection) {
            // Calculate the diffuse light.
            float cosDiff = dot(lightDir, normal);
            if(cosDiff < FLT_EPSILON) cosDiff = 0.0f;
            diffLight += lights[lightID].color * cosDiff
                * material->diffuseCoef * att;

            // Calculate the specular light.

            // Halfway vector.
            float4 halfway = normalize(lightDir + (-1.0f * eyeDir));

            // Cos of the angle between the halfway and the normal.
            float cosSpec = dot(halfway, normal);
            if(cosSpec < FLT_EPSILON) cosSpec = 0.0f;

            // Shininess.
            cosSpec = pow(cosSpec, material->specularExp);

            specLight += lights[lightID].color * cosSpec
                * material->specularCoef * att;
        }
    }
    *ambDiffColor += NumLights * diffLight / NumLightSamples;
    *specularColor += NumLights * specLight / NumLightSamples;
}

float4 calculateFinalColor(State *t) {
    // Calculate the final color.
    // Ambient and diffuse colors are multiplied, the others need a sum.
    float4 outColor = getTextureColor(t->textureType, t->textureID,
            t->intersection);
    outColor *= t->ambientDiffuseColor;
    outColor += t->specularColor + t->reflectionColor + t->transmissionColor;

    return clamp(outColor, 0.0f, 1.0f);
}

void radianceStage0(Stack *stack, RetStack *retStack, State *t) {
    // See if the ray intersects anything.
    t->iType = trace(t->origin, t->dir, t->exclType, t->exclID, 0, &t->id,
            &t->intersection, &t->normal, &t->inside);

    if(t->iType == NoIntersection) { // Don't need to do anything anymore.
        float4 *r = retStackTop(retStack);
        *r = ambientLight.color;
        retStackPush(retStack);
        return;
    }

    // Go to stage that estimates the light color.
    State *newT = stackTop(stack);
    initStage1(newT, &t->dir, &t->intersection, t->iType, t->id, &t->normal,
            t->inside, t->seed);
    stackPush(stack);
}

void radianceStage1(Stack *stack, RetStack *retStack, State *t) {
    // Get the ids from the object.
    getObjectIDs(t->iType, t->id, &t->materialID, &t->textureType,
            &t->textureID);

    // Calculate the local component of the color.
    calculateLocalColor(t->dir, t->iType, t->id, t->intersection, t->normal,
            &materials[t->materialID], t->seed, &t->ambientDiffuseColor,
            &t->specularColor);

/*
    t->reflectionColor = (float4) (0.0f);
    t->transmissionColor = (float4) (0.0f);
    if(materials[t->materialID].reflectionCoef > 0.0f) {
        // Reflected component added in recursively.
        float4 reflDir = getReflectionDirection(t->dir, t->normal);
        t->stage = 1;
        stackPush(stack); // Set this for when the recursion returns.

        State *newT = stackTop(stack);
        initState(newT, t->intersection, reflDir, t->iType, t->id,
                t->depth - 1, 0);
        stackPush(stack); // new iteration.
        return;
    }
*/
    radianceStage3(retStack, t);
/*
    retStackPop(retStack);
    t->reflectionColor = *retStackTop(retStack);
    t->reflectionColor *= materials[t->materialID].reflectionCoef;

    // If outside the object invert the refraction rate.
    float refrRate = materials[t->materialID].refractionRate;
    if(!t->inside)
        refrRate = 1.0f / refrRate;

    // Transmission component added in recursively.
    float4 transDir;
    if(getTransmissionDirection(refrRate, t->dir, t->normal, &transDir)
            && materials[t->materialID].transmissionCoef > 0.0f) {
        t->stage = 2;
        stackPush(stack); // Set this for when the recursion returns.

        State *newT = stackTop(stack);
        initState(newT, t->intersection, transDir, t->iType, t->id,
                t->depth - 1, 0);
        stackPush(stack); // New iteration.
        return;
    }

    radianceStage3(retStack, t);
    */
}

void radianceStage2(RetStack *retStack, State *t) {
    retStackPop(retStack);
    t->transmissionColor = *retStackTop(retStack);
    t->transmissionColor *= materials[t->materialID].transmissionCoef;

    radianceStage3(retStack, t);
}

void radianceStage3(RetStack *retStack, State *t) {
        float4 *r = retStackTop(retStack);
        *r = calculateFinalColor(t);
        retStackPush(retStack);
}

float4 radiance(float4 *argOrigin, float4 *argDir, uint2 *seed) {
    Stack stack; // Recursion stack.
    RetStack retStack; // Return stack.
    State *t; // Top state.

    stackInit(&stack);
    retStackInit(&retStack);

    t = stackTop(&stack);
    initStage0(t, *argOrigin, *argDir, NoIntersection, seed);
    stackPush(&stack);

    // Simulated recursion.
    while(!stackEmpty(&stack)) {
        stackPop(&stack);
        t = stackTop(&stack);
        switch(t->stage) {
            case 0: radianceStage0(&stack, &retStack, t); break;
            case 1: radianceStage1(&stack, &retStack, t); break;
            case 2: radianceStage2(&retStack, t); break;
        }
    }

    // Return the top of the stack.
    retStackPop(&retStack);
    return *retStackTop(&retStack);
}

#endif // !RADIANCE_CL
