#ifndef BRDF_CL
#define BRDF_CL

/*
 * Returns the normal base.
 */
void getNormalBase(float4 normal, float4 *u, float4 *v, float4 *w);

/*
 * Returns a random direction with importance sampling to the diffuse BRDF
 * and outputs the f function also.
 */
float4 brdfDiffuse(float4 normal, __constant Material *material, float *f,
        uint2 *seed);

/*
 * Returns a random direction with importance sampling to the specular BRDF
 * and outputs the f function also.
 */
float4 brdfSpecular(float4 wo, float4 normal, __constant Material *material,
        float *f, uint2 *seed);

/**
 * Returns the incoming direct radiance from the direction given.
 */
float4 directRadiance(float4 origin, float4 dir, IntersectionType exclType,
        int exclID);

void getNormalBase(float4 normal, float4 *u, float4 *v, float4 *w) {
    *w = normal;
    *u = normalize(cross(fabs(w->x) > 0.1f ? (float4)(0.0f, 1.0f, 0.0f, 0.0f)
            : (float4)(1.0f, 0.0f, 0.0f, 0.0f), *w));
    *v = cross(*w, *u);
}

float4 brdfDiffuse(float4 normal, __constant Material *material, float *f,
        uint2 *seed) {
    // Get the normal base.
    float4 u, v, w;
    getNormalBase(normal, &u, &v, &w);

    // Generate random direction based on pdf =  1/pi * cos(thetai)
    float u1 = randf(seed), u2 = randf(seed);
    float theta = acos(sqrt(u1)), phi = 2 * M_PI * u2;

    // Convert from spherical coordinates and add the base.
    float4 dir = normalize((float4)(
        u * sin(theta) * cos(phi) +
        v * sin(theta) * sin(phi) +
        w * cos(theta)
    ));

    *f = material->diffuseCoef * M_1_PI;

    return dir;
}

float4 brdfSpecular(float4 wo, float4 normal, __constant Material *material,
        float *f, uint2 *seed) {
    // Get the normal base.
    float4 u, v, w;
    getNormalBase(normal, &u, &v, &w);

    // Generate random direction based on pdf = (n+8)/8pi * cos^n(alpha)
    float u1 = randf(seed), u2 = randf(seed);
    float e = 1.0f / (material->specularExp + 1.0f);
    float theta = acos(pow(u1, e)), phi = 2.0f * M_PI * u2;

    // Convert from spherical coordinates and add the base.
    float4 dir = normalize((float4)(
        u * sin(theta) * cos(phi) +
        v * sin(theta) * sin(phi) +
        w * cos(theta)
    ));

    // Cos(alpha)
    float cosAlpha = dot(dir, wo);
    if(cosAlpha < 0.0f) cosAlpha = cos(M_PI/2);

    *f = material->specularCoef * (material->specularExp + 8.0f)
        * (8.0f * M_1_PI) * pow(cosAlpha, material->specularCoef);

    return dir;
}

float4 directRadiance(float4 origin, float4 dir, IntersectionType exclType,
        int exclID) {
    // See if there is a light intersection.
    int id;
    float4 interPos;
    bool hit = traceLight(origin, dir, &id, &interPos);
    if(!hit) // No lights.
        return (float4)(0.0f, 0.0f, 0.0f, 0.0f);

    // If the light isn't occluded, return the light, else 0.
    if(traceObjects(origin, dir, exclType, exclID, &interPos,
            0, 0, 0, 0) == NoIntersection) {
        // Calculate the light attenuation.
        float dist = distance(interPos, origin);
        float att = 1.0f / (lights[id].constantAtt
                + dist * lights[id].linearAtt
                + pow(dist, 2) * lights[id].quadraticAtt);

        return lights[id].color * att;
    }

    return (float4) (0.0f, 0.0f, 0.0f, 0.0f);
}

#endif // !BRDF_CL
