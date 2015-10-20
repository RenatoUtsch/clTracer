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

#define STACK_SIZE (MAX_DEPTH * MAX_DEPTH * MAX_DEPTH)

/// Value returned by the trace function.
typedef enum IntersectionType {
    NoIntersection,
    SphereIntersection,
    PolyhedronIntersection
} IntersectionType;

/// Recursion state.
typedef struct State {
    float4 origin, dir, intersection, normal;
    float4 ambientDiffuseColor, specularColor;
    float4 reflectionColor, transmissionColor;
    float4 outColor;
    int id, exclID, materialID, textureID, depth, stage;
    IntersectionType exclType, iType;
    TextureType textureType;
    bool inside;
} State;

/// Stack used for recursion.
typedef struct Stack {
    State data[STACK_SIZE];
    int top;
} Stack;

/// Stack of the return values.
typedef struct RetStack {
    float4 data[STACK_SIZE];
    int top;
} RetStack;

/**
 * Tries to intersect with a sphere.
 * @param id ID of the sphere to try to intersect.
 * @param origin Origin of the ray.
 * @param dir Direction of the ray.
 * @param maxT Maximum parametric value. If the intersection generates a bigger
 * parametric value, it is discarded.
 * @param inside Set to true if the intersection is inside the sphere and to
 * false otherwise.
 * @return The parametric value used to calculate the intersection position.
 */
float sphereIntersection(int id, float4 origin, float4 dir, float maxT,
        bool *inside);

/**
 * Tries to instersect with a polyhedron.
 * @param id ID of the polyhedron to try to intersect.
 * @param origin Origin of the ray.
 * @param dir Direction of the ray.
 * @param maxT Maximum parametric value. If the intersection generates a bigger
 * parametric value, it is discarded.
 * @param normal Set to the plane's normal.
 * @return The parametric value used to calculate the intersection position.
 */
float polyhedronIntersection(int id, float4 origin, float4 dir, float maxT,
        float4 *normal);

/**
 * Traces the ray cast by sample() and sees if it intersects anything. Returns
 * what happened.
 * @param origin The ray origin.
 * @param direction The ray direction.
 * @param exclID ID of an object to be excluded from the search. Set to -1 to
 * search all objects.
 * @param exclType Type of an object to be excluded from the search. Set to
 * 0 to search all objects.
 * @param endPos If the object is found after this distance, it will not be
 * counted. Set to 0 to search for all objects.
 * @param outIntersectionID Set to the ID of the intersected object. Set to
 * 0 to ignore.
 * @param outIntersection Set to the point of intersection. Set to 0 to ignore.
 * @param outIntersectionNormal Set to the normal at the point of intersection.
 * Set to 0 to ignore.
 * @param outInside Set to true if the ray is inside (the sphere) and to false
 * otherwise. Set to 0 to ignore.
 * @return The type of intersection.
 */
IntersectionType trace(float4 origin, float4 direction,
        IntersectionType exclType, int exclID, __constant float4 *endPos,
        int *outIntersectionID, float4 *outIntersection,
        float4 *outIntersectionNormal, bool *outInside);

/**
 * Returns the color of the given texture type / ID.
 * @param type The texture type.
 * @param id The ID of the texture.
 * @param p The point of intersection to calculate the color.
 */
float4 getTextureColor(TextureType type, int id, float4 p);

/**
 * Returns the correct object IDs given the intersection type and object id.
 * @param iType Type of the intersected object.
 * @param id ID of the object.
 * @param materialID Output material id.
 * @param textureType Output texture type.
 * @param textureID Output texture id.
 */
void getObjectIDs(IntersectionType iType, int id, int *materialID,
        TextureType *textureType, int *textureID);

/**
 * Returns the reflection direction.
 * @param dir Ray direction.
 * @param normal Normal at the intersection.
 */
float4 getReflectionDirection(float4 dir, float4 normal);

/**
 * Returns the transmission direction.
 * @param refrRate The refraction rate.
 * @param dir Direction of the ray.
 * @param normal Normal at the intersection.
 * @param transDir Output transmission direction.
 * @return If there is indeed transmission or not.
 */
bool getTransmissionDirection(float refrRate, float4 dir, float4 normal,
        float4 *transDir);

/* Stack functions. */
void stackInit(Stack *stack);
State *stackTop(Stack *stack);
void stackPush(Stack *stack);
void stackPop(Stack *stack);
bool stackEmpty(Stack *stack);
void initState(State *t, float4 origin, float4 dir,
        IntersectionType exclType, int exclID, int depth, int stage);
void retStackInit(RetStack *retStack);
float4 *retStackTop(RetStack *retStack);
void retStackPush(RetStack *retStack);
void retStackPop(RetStack *retStack);

/**
 * Calculates the local color component.
 * @param eyeDir Direction from the eye (camera).
 * @param iType Type of the first intersection.
 * @param id ID of the object of the first intersection.
 * @param intersection Coordinates of intersection.
 * @param normal Normal at intersection point.
 * @param material Material of the intersected object.
 * @param ambDiffColor output ambient + diffuse color.
 * @param specularCOlor output specular color.
 */
void calculateLocalColor(float4 eyeDir, IntersectionType iType, int id,
        float4 intersection, float4 normal, __constant Material *material,
        float4 *ambDiffColor, float4 *specularColor);

/**
 * Properly runs the sample.
 * @param origin Ray origin.
 * @param dir Ray direction.
 * @return Color that was sampled.
 */
float4 runSample(float4 *origin, float4 *dir);

float sphereIntersection(int id, float4 origin, float4 dir, float maxT,
        bool *inside)
{
    float4 e = spheres[id].center - origin;
    float tca = dot(e, dir);
    float d2 = dot(e, e) - pow(tca, 2);
    if(d2 > spheres[id].radius2) return -1.0f; // No intersection.

    float thc = sqrt(spheres[id].radius2 - d2);
    float t1 = tca - thc;
    float t2 = tca + thc;

    // t1 is always smaller than t2 since t1 subtracts thc.
    if(t1 > 0.0f && t1 < maxT) {
        *inside = false;
        return t1;
    }
    else if(t2 > 0.0f && t2 < maxT) {
        *inside = true;
        return t2;
    }

    return -1.0f;
}

float polyhedronIntersection(int id, float4 origin, float4 dir, float maxT,
        float4 *normal) {
    int facesBegin = polyhedrons[id].facesBegin;
    float t;
    float t0 = 0.0f, t1 = FLT_MAX;
    float4 nT0, nT1;

    for(int i = 0; i < polyhedrons[id].numFaces; ++i) {
        float4 p0 = (float4) (origin.xyz, 0.0f);
        float4 n = (float4) (polyhedronFaces[facesBegin + i].xyz, 0.0f);
        float dn = dot(dir, n); // hu
        float val = dot(p0, n) + polyhedronFaces[facesBegin + i].w; // hp

        if(fabs(dn) <= FLT_EPSILON) {
            if(val > FLT_EPSILON)
                t1 = -1.0f;
        }
        if(dn > FLT_EPSILON) {
            t = -val / dn;
            if(t < t1) {
                // Replace the furthest point.
                t1 = t;
                nT1 = n;
            }
        }
        if(dn < -FLT_EPSILON) {
            t = -val / dn;
            if(t > t0) {
                t0 = t;
                nT0 = n;
            }
        }
    }

    if(t1 < t0)
        return -1.0f;
    if(fabs(t0) <= FLT_EPSILON && (t1 >= t0) && t1 < FLT_MAX) {
        *normal = normalize(-1.0f * nT1);
        if(t1 < maxT)
            return t1;
        else
            return -1.0f;
    }
    if(t0 > FLT_EPSILON && t1 >= t0) {
        *normal = normalize(nT0);
        if(t0 < maxT)
            return t0;
        else
            return -1.0f;
    }

    return -1.0f;
}

IntersectionType trace(float4 origin, float4 direction,
        IntersectionType exclType, int exclID, __constant float4 *endPos,
        int *outIntersectionID, float4 *outIntersection,
        float4 *outIntersectionNormal, bool *outInside)
{
    float4 closestNormal;
    float closestT = FLT_MAX;
    int closestID;
    IntersectionType closestType = NoIntersection;
    bool closestInside = false;
    bool inside;

    // Find the maximum t.
    // If origin + t * dir = endPos, then t = (endPos - origin) / dir.
    float maxT;
    if(endPos)
        maxT = (endPos->x - origin.x) / direction.x;
    else
        maxT = FLT_MAX;

    // Intersect with spheres.
    for(int i = 0; i < numSpheres; ++i) {
        if(exclType == SphereIntersection && i == exclID) continue;

        float t = sphereIntersection(i, origin, direction, maxT, &inside);

        if(t > FLT_EPSILON && t < closestT) {
            closestT = t;
            closestType = SphereIntersection;
            closestID = i;
            closestInside = inside;
        }
    }
    // Intersect with polyhedrons.
    for(int i = 0; i < numPolyhedrons; ++i) {
        if(exclType == PolyhedronIntersection && i == exclID) continue;

        float4 normal;
        float t = polyhedronIntersection(i, origin, direction, maxT, &normal);

        if(t > FLT_EPSILON && t < closestT) {
            closestT = t;
            closestType = PolyhedronIntersection;
            closestID = i;
            closestNormal = normal;
        }
    }

    if(closestType != NoIntersection) {
        if(outIntersectionID)
            *outIntersectionID = closestID;
        if(outIntersection)
            *outIntersection = origin + closestT * direction;

        if(closestType == SphereIntersection) {
            if(outIntersectionNormal) {
                closestNormal = normalize(*outIntersection
                        - spheres[closestID].center);
                if(closestInside) // Invert the normal.
                    closestNormal *= -1.0f;

                *outIntersectionNormal = closestNormal;
            }
            if(outInside)
                *outInside = closestInside;

            return SphereIntersection;
        }

        if(closestType == PolyhedronIntersection) {
            if(outIntersectionNormal)
                *outIntersectionNormal = closestNormal;

            if(outInside)
                *outInside = false;

            return PolyhedronIntersection;
        }
    }

    return NoIntersection;
}

float4 getTextureColor(TextureType type, int id, float4 p) {
    switch(type) {
        case SolidTextureType:
            return solidTextures[id].color;

        case CheckerTextureType: {
            int val = floor(p.x / checkerTextures[id].size)
                + floor(p.y / checkerTextures[id].size)
                + floor(p.z / checkerTextures[id].size);
            val = val % 2;

            if(!val)
                return checkerTextures[id].color1;
            else
                return checkerTextures[id].color2;
        }

        case MapTextureType: {
            float s = dot(mapTextures[id].p0, p);
            float r = dot(mapTextures[id].p1, p);
            int i = (int)(r * mapTextures[id].height) % mapTextures[id].height;
            int j = (int)(s * mapTextures[id].width) % mapTextures[id].width;
            if(i < 0) i += mapTextures[id].height;
            if(j < 0) j += mapTextures[id].width;

            int pos = mapTextures[id].dataBegin;
            pos += i * mapTextures[id].height + j;
            return mapData[pos];
        }
    }
}

void getObjectIDs(IntersectionType iType, int id, int *materialID,
        TextureType *textureType, int *textureID) {
    switch(iType) {
        case NoIntersection:
            break; // Shouldn't happen.

        case SphereIntersection:
            *materialID = spheres[id].materialID;
            *textureID = spheres[id].textureID;
            *textureType = spheres[id].textureType;
            break;

        case PolyhedronIntersection:
            if(numPolyhedrons) { // This if is because of an Intel compiler bug.
                *materialID = polyhedrons[id].materialID;
                *textureID = polyhedrons[id].textureID;
                *textureType = polyhedrons[id].textureType;
            }
            break;
    }
}

float4 getReflectionDirection(float4 dir, float4 normal)
{
    // Get the inversion of the direction.
    float c = dot(-1.0f * dir, normal);

    return normalize(dir + (2 * normal * c));
}

bool getTransmissionDirection(float refrRate, float4 dir, float4 normal,
        float4 *transDir)
{
    float4 invDir = -1.0f * dir;
    float c1 = dot(invDir, normal);
    float c2 = 1.0f - pow(refrRate, 2) * (1.0f - pow(c1, 2));
    float marreta = 3.0f;

    if(c2 < -FLT_EPSILON) { // Total internal reflection.
        *transDir = normalize((2 * normal * c1) - invDir);
        return true;
    }
    else if(c2 > FLT_EPSILON) { // Refraction.
        c2 = sqrt(c2 + marreta); // Marretagem.
        *transDir = normalize(normal * (refrRate * c1 - c2) + dir * refrRate);
        return true;
    }
    else { // Parallel ray.
        return false;
    }
}

void stackInit(Stack *stack) {
    stack->top = 0;
}

State *stackTop(Stack *stack) {
    return &stack->data[stack->top];
}

void stackPush(Stack *stack) {
    ++stack->top;
}

void stackPop(Stack *stack) {
    --stack->top;
}

bool stackEmpty(Stack *stack) {
    return stack->top == 0;
}

void initState(State *t, float4 origin, float4 dir,
        IntersectionType exclType, int exclID, int depth, int stage) {
    t->origin = origin;
    t->dir = dir;
    t->exclType = exclType;
    t->exclID = exclID;
    t->depth = depth;
    t->stage = stage;
}

void retStackInit(RetStack *retStack) {
    retStack->top = 0;
}

float4 *retStackTop(RetStack *retStack) {
    return &retStack->data[retStack->top];
}

void retStackPush(RetStack *retStack) {
    ++retStack->top;
}

void retStackPop(RetStack *retStack) {
    --retStack->top;
}


void calculateLocalColor(float4 eyeDir, IntersectionType iType, int id,
        float4 intersection, float4 normal, __constant Material *material,
        float4 *ambDiffColor, float4 *specularColor)
{
    *ambDiffColor = lights[0].color * material->ambientCoef;
    *specularColor = (float4) (0.0f);

    // Trace a ray to all light sources and calculate the local light component.
    // Start at 1 because not counting ambient light.
    for(int i = 1; i < numLights; ++i) {
        float4 lightDir = normalize(lights[i].pos - intersection);
        float dist = distance(lights[i].pos, intersection);

        // Calculate the light attenuation.
        float att = 1.0f / (lights[i].constantAtt
                + dist * lights[i].linearAtt
                + pow(dist, 2) * lights[i].quadraticAtt);

        // If the feeler ray does not intersect the surface of any object before
        // reaching the light, then the object is not shadowed.
        // If the object intersects, then just use the ambient color and ignore
        // the others.
        if(trace(intersection, lightDir, iType, id, &lights[i].pos,
                    0, 0, 0, 0) == NoIntersection) {
            // Calculate the diffuse light.
            float cosDiff = dot(lightDir, normal);
            if(cosDiff < FLT_EPSILON) cosDiff = 0.0f;

            *ambDiffColor += lights[i].color * cosDiff
                * material->diffuseCoef * att;

            // Calculate the specular light.

            // Halfway vector.
            float4 halfway = normalize(lightDir + (-1.0f * eyeDir));

            // Cos of the angle between the halfway and the normal.
            float cosSpec = dot(halfway, normal);
            if(cosSpec < FLT_EPSILON) cosSpec = 0.0f;

            // Shininess.
            cosSpec = pow(cosSpec, material->specularExp);

            *specularColor += lights[i].color * cosSpec
                * material->specularCoef * att;
        }
    }
}

float4 runSample(float4 *argOrigin, float4 *argDir) {
    Stack stack; // Recursion stack.
    RetStack retStack; // Return stack.
    State *t; // Top state.
    State *newT; // New top state.
    float4 *r; // Return state.

    stackInit(&stack);
    retStackInit(&retStack);

    t = stackTop(&stack);
    initState(t, *argOrigin, *argDir, NoIntersection, -1, MAX_DEPTH, 0);
    stackPush(&stack);

    while(!stackEmpty(&stack)) {
        stackPop(&stack);
        t = stackTop(&stack);
        switch(t->stage) {
            case 0: goto Stage0;
            case 1: goto Stage1;
            case 2: goto Stage2;
        }

Stage0:
        // See if the ray intersects anything.
        t->iType = trace(t->origin, t->dir, t->exclType, t->exclID, 0, &t->id,
                &t->intersection, &t->normal, &t->inside);

        if(t->iType == NoIntersection) { // Don't need to do anything anymore.
            r = retStackTop(&retStack);
            *r = lights[0].color;
            retStackPush(&retStack);
            continue;
        }

        getObjectIDs(t->iType, t->id, &t->materialID, &t->textureType,
                &t->textureID);

        // Calculate the local component of the color.
        calculateLocalColor(t->dir, t->iType, t->id, t->intersection, t->normal,
                &materials[t->materialID], &t->ambientDiffuseColor,
                &t->specularColor);

        t->reflectionColor = (float4) (0.0f);
        t->transmissionColor = (float4) (0.0f);
        if(t->depth > 0) {
            // Reflected component added in recursively.
            float4 reflDir = getReflectionDirection(t->dir, t->normal);
            t->stage = 1;
            stackPush(&stack);

            newT = stackTop(&stack);
            initState(newT, t->intersection, reflDir, t->iType, t->id,
                    t->depth - 1, 0);
            stackPush(&stack);
            continue;
Stage1:
            retStackPop(&retStack);
            t->reflectionColor = *retStackTop(&retStack);
            t->reflectionColor *= materials[t->materialID].reflectionCoef;

            // If outside the object invert the refraction rate.
            float refrRate = materials[t->materialID].refractionRate;
            if(!t->inside)
                refrRate = 1.0f / refrRate;

            // Transmission component added in recursively.
            float4 transDir;
            if(getTransmissionDirection(refrRate, t->dir, t->normal, &transDir)) {
                t->stage = 2;
                stackPush(&stack);

                newT = stackTop(&stack);
                initState(newT, t->intersection, transDir, t->iType, t->id,
                        t->depth - 1, 0);
                stackPush(&stack);
                continue;
Stage2:
                retStackPop(&retStack);
                t->transmissionColor = *retStackTop(&retStack);
                t->transmissionColor *= materials[t->materialID].transmissionCoef;
            }
        }

        // Calculate the final color.
        // Ambient and diffuse colors are multiplied, the others need a sum.
        float4 outColor = getTextureColor(t->textureType, t->textureID,
                t->intersection);
        outColor *= t->ambientDiffuseColor;
        outColor += t->specularColor + t->reflectionColor + t->transmissionColor;

        r = retStackTop(&retStack);
        *r = clamp(outColor, 0.0f, 1.0f);
        retStackPush(&retStack);
    }

    retStackPop(&retStack);
    return *retStackTop(&retStack);
}

/**
 * Samples a ray from origin through direction.
 */
__kernel void sample(__constant float4 *cOrigin, __constant float4 *topLeft,
        __constant float4 *up, __constant float4 *right, float pixelWidth,
        float pixelHeight, __write_only image2d_t out)
{
    int2 coord = (int2) (get_global_id(0), get_global_id(1));
    float4 origin = *cOrigin;

    // First get the point position.
    float4 dir = *topLeft + (*right * (coord.x * pixelWidth))
        - (*up * (coord.y * pixelHeight));

    // Now make it a direction vector.
    dir = normalize(dir - origin);

    write_imagef(out, coord, runSample(&origin, &dir));
}
