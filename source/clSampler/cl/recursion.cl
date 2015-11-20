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

#ifndef RECURSION_CL
#define RECURSION_CL

#include "intersection.cl"

#define STACK_SIZE (MAX_DEPTH * MAX_DEPTH * MAX_DEPTH)

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

#endif // !RECURSION_CL
