#ifndef BRDF_CL
#define BRDF_CL

#include "direction.cl"

/**
 * Given the ray direction, intersection normal, material and if it is inside
 * the object, returns a new ray direction, the BRDF f function and the pdf.
 * Returns if a new direction was generated or if is to stop recursion.
 * The BRDF f value still needs to be multiplied by the albedo.
 */
bool brdf(float4 dir, float4 normal, float4 albedo, __constant Material *mat,
        bool inside, uint2 *seed, float4 *newDir, float4 *f, float *pdf);

/// BRDF for the diffuse component.
bool brdfDiffuse(float4 normal, float4 albedo, __constant Material *mat,
        uint2 *seed, float4 *newDir, float4 *f, float *pdf);

/// BRDF for the specular component.
bool brdfSpecular(float4 dir, float4 normal, float4 albedo,
        __constant Material *mat, uint2 *seed, float4 *newDir, float4 *f,
        float *pdf);

/// BRDF for the reflection component.
bool brdfReflection(float4 dir, float4 normal, float4 albedo,
        __constant Material *mat, float4 *newDir, float4 *f, float *pdf);

/// BRDF for the transmission component.
bool brdfTransmission(float4 dir, float4 normal, float4 albedo,
        __constant Material *mat, bool inside, float4 *newDir, float4 *f,
        float *pdf);

/**
 * Returns the normal base.
 */
void getNormalBase(float4 normal, float4 *u, float4 *v, float4 *w);

bool brdf(float4 dir, float4 normal, float4 albedo, __constant Material *mat,
        bool inside, uint2 *seed, float4 *newDir, float4 *f, float *pdf) {
    float u = randf(seed);
    float c = 0.0f;

    // Choose which brdf to use based on the coefficients. Note that all
    // coefficients must sum to <= 1.0f for energy conservation.
    if(u < (c += mat->diffuseCoef)) // Sample diffuse BRDF.
        return brdfDiffuse(normal, albedo, mat, seed, newDir, f, pdf);
    else if(u < (c += mat->specularCoef)) // Sample specular BRDF.
        return brdfSpecular(dir, normal, albedo, mat, seed, newDir, f, pdf);
    else if(u < (c += mat->reflectionCoef)) // Sample reflection BRDF.
        return brdfReflection(dir, normal, albedo, mat, newDir, f, pdf);
    else if(u < (c += mat->transmissionCoef)) // Sample transmission BRDF.
        return brdfTransmission(dir, normal, albedo, mat, inside, newDir, f, pdf);
    else // No contribution.
        return false;
}

void getNormalBase(float4 normal, float4 *u, float4 *v, float4 *w) {
    *w = normal;
    *u = normalize(cross(fabs(w->x) > 0.1f ? (float4)(0.0f, 1.0f, 0.0f, 0.0f)
            : (float4)(1.0f, 0.0f, 0.0f, 0.0f), *w));
    *v = cross(*w, *u);
}

/// BRDF for the diffuse component.
bool brdfDiffuse(float4 normal, float4 albedo, __constant Material *mat,
        uint2 *seed, float4 *newDir, float4 *f, float *pdf) {
    float4 u, v, w;
    getNormalBase(normal, &u, &v, &w);

    // Generate random importance sampled direction based on Blinn-Phong pdf.
    float u1 = randf(seed), u2 = randf(seed);
    float theta = 2 * M_PI * u1, phi = sqrt(u2);

    // Convert from spherical coordinates and add the base.
    *newDir = normalize((float4) (
        u * cos(theta) * phi +
        v * sin(theta) * phi +
        w * sqrt(1.0f - u2)
    ));

    float cosND = max(dot(normal, *newDir), 0.0f);

    *f = albedo * (float) (mat->diffuseCoef * M_1_PI * cosND);
    *pdf = cosND * M_1_PI;

    if(fabs(*pdf) < FLT_EPSILON)
        return false;
    else
        return true;
}

/// BRDF for the specular component.
bool brdfSpecular(float4 dir, float4 normal, float4 albedo,
        __constant Material *mat, uint2 *seed, float4 *newDir, float4 *f,
        float *pdf) {
    float4 u, v, w;
    getNormalBase(normal, &u, &v, &w);

    // Generate random importance sampled direction based on Blinn-Phong pdf.
    float u1 = randf(seed), u2 = randf(seed);
    float e = 1.0f / (mat->specularExp + 1.0f);
    float theta = acos(pow(u1, e)), phi = 2.0f * M_PI * u2;

    // Convert from spherical coordinates and add the base.
    *newDir = normalize((float4) (
        u * sin(theta) * cos(phi) +
        v * sin(theta) * sin(phi) +
        w * cos(theta)
    ));

    float cosAlpha = dot(dir, *newDir);
    if(cosAlpha < 0.0f) cosAlpha = cos(M_PI / 2.0f);

    *f = albedo * (float) (mat->specularCoef * ((mat->specularExp + 8.0f)
        / (8.0f * M_PI)) * pow(cosAlpha, mat->specularExp));

    *pdf = ((mat->specularExp + 2.0f) / (2.0f * M_PI))
        * pow(cosAlpha, mat->specularExp);

    if(fabs(*pdf) < FLT_EPSILON)
        return false;
    else
        return true;
}

/// BRDF for the ideal reflection component.
bool brdfReflection(float4 dir, float4 normal, float4 albedo,
        __constant Material *mat, float4 *newDir, float4 *f, float *pdf) {
    *newDir = getReflectionDirection(dir, normal);
    *f = albedo * mat->reflectionCoef;
    *pdf = 1.0f;

    return true;
}

/// BRDF for the ideal transmission component.
bool brdfTransmission(float4 dir, float4 normal, float4 albedo,
        __constant Material *mat, bool inside, float4 *newDir, float4 *f,
        float *pdf) {
    float refrRate = mat->refractionRate;
    if(!inside)
        refrRate = 1.0f / refrRate;

    bool result = getTransmissionDirection(refrRate, dir, normal, inside, newDir);
    if(result) {
        *f = albedo * mat->transmissionCoef;
        *pdf = 1.0f;
        return true;
    }

    return false;
}

#endif // !BRDF_CL
