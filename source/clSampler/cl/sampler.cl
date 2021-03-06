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

#include "radiance.cl"
#include "random.cl"

/**
 * Samples a ray from origin through direction.
 */
__kernel void sample(__constant float4 *camera, __constant float4 *topLeft,
        __constant float4 *up, __constant float4 *right, uint2 seed,
        __write_only image2d_t out)
{
    int2 coord = (int2) (get_global_id(0), get_global_id(1));
    float4 origin = *camera;
    float4 color = (float4) (0.0f);

    // Init the PRNG seed.
    seed.x += get_global_size(0) * coord.y + coord.x;
    seed.y += get_global_size(0) * coord.y + coord.x;

    // First get the pixel position.
    float4 pixelPos = *topLeft + (*right * (coord.x * PixelWidth))
        - (*up * (coord.y * PixelHeight));

    float hPart = PixelHeight / AALevel;
    float wPart = PixelWidth / AALevel;
    for(int i = 0; i < AALevel; ++i) {
        for(int j = 0; j < AALevel; ++j) {
            for(int k = 0; k < NumSamples; ++k) {
                // Get  the position of the subpixel.
                float4 point = pixelPos + *up * i * hPart + *right * j * wPart;

                // Get the position at the inside of the subpixel.
                point += *up * (randf(&seed) * hPart)
                    + *right * (randf(&seed) * wPart);

                // Now make it a direction vector.
                float4 dir = normalize(point - origin);

                color += radiance(&origin, &dir, &seed);
            }
        }
    }
    color /= AALevel * AALevel * NumSamples;

    write_imagef(out, coord, color);
}
