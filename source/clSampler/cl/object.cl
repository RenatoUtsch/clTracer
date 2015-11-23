#ifndef OBJECT_CL
#define OBJECT_CL

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
 * Returns if a sphere emits.
 */
inline bool sphereEmits(int id) {
    return spheres[id].emission.x > 0.0f || spheres[id].emission.y > 0.0f
        || spheres[id].emission.z > 0.0f;
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
            if(NumPolyhedrons) { // This if is because of an Intel compiler bug.
                *materialID = polyhedrons[id].materialID;
                *textureID = polyhedrons[id].textureID;
                *textureType = polyhedrons[id].textureType;
            }
            break;
    }
}

#endif // !OBJECT_CL
