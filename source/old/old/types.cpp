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

#include "types.hpp"
#include "scene.hpp"
#include "math/math.hpp"
#include <stdexcept>

const Texture &Object::texture() const {
    return scene->textures[textureId];
}

const Material &Object::material() const {
    return scene->materials[materialId];
}

Color Object::color(const Point &p) const {
    const Texture &tex = texture();
    int val;
    double s, r;
    int i, j;

    switch(tex.type) {
        case SolidTextureType:
            return tex.solid.color;

        case CheckerTextureType:
            val = floor(p.x / tex.checker.size)
                + floor(p.y / tex.checker.size)
                + floor(p.z / tex.checker.size);
            val = val % 2;

            if(!val)
                return tex.checker.color1;
            else
                return tex.checker.color2;

        case MapTextureType:
            s = tex.map.p0.x * p.x
                + tex.map.p0.y * p.y
                + tex.map.p0.z * p.z
                + tex.map.p0.w;
            r = tex.map.p1.x * p.x
                + tex.map.p1.y * p.y
                + tex.map.p1.z * p.z
                + tex.map.p1.w;
            i = (int)(r * tex.map.ppm.height) % tex.map.ppm.height;
            j = (int)(s * tex.map.ppm.width) % tex.map.ppm.width;
            if(i < 0) i += tex.map.ppm.height;
            if(j < 0) j += tex.map.ppm.width;
            return tex.map.ppm.data.at(i).at(j);
    }

    throw std::runtime_error("Invalid texture type");
}
