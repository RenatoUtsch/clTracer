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

#ifndef RANDOM_CL
#define RANDOM_CL

/**
 * Uniform random number generator.
 * Returns a random uint.
 * This rand() is a implementation of the MWC64X PRNG.
 * http://cas.ee.ic.ac.uk/people/dt10/research/rngs-gpu-mwc64x.html
 */
uint rand(uint2 *state);

/**
 * Same as rand(), but converts the result to an uint on the range [0, n].
 */
uint randn(uint n, uint2 *state);

/**
 * Same as rand(), but converts the result to a float on the range [0, 1].
 */
float randf(uint2 *state);

/**
 * Generates a random value in a circle.
 * @param center Center of the circle.
 * @param right Right vector of the circle.
 * @param up Up vector of the circle.
 * @param radius Radius of the circle.
 */
float4 randcircle(float4 center, float4 right, float4 up, float radius,
        uint2 *state);

/**
 * Generates a random direction in a unit sphere.
 * @param center Center of the sphere.
 */
float4 randsphere(float4 center, uint2 *state);

uint rand(uint2 *state) {
    enum { A=4294883355U };
    uint x=(*state).x, c=(*state).y;  // Unpack the state
    uint res=x^c;                     // Calculate the result
    uint hi=mul_hi(x,A);              // Step the RNG
    x=x*A+c;
    c=hi+(x<c);
    *state=(uint2)(x,c);              // Pack the state back up

    return res;
}

uint randn(uint n, uint2 *state) {
    return rand(state) % n;
}

float randf(uint2 *state) {
    // Convert to range and return result.
    return (float) rand(state) / UINT_MAX;
}

float4 randcircle(float4 center, float4 right, float4 up, float radius,
        uint2 *state) {
    float4 point;

    // TODO: this is rejection sampling. Should implement something faster.
    do {
        point = center + (randf(state) * 2 * radius - radius) * right
            + (randf(state) * 2 * radius - radius) * up;
    } while(fabs(distance(center, point)) > radius);

    return point;
}

float4 randsphere(float4 center, uint2 *state) {

}

#endif // !RANDOM_CL
