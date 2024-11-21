#ifndef BRDF_H
#define BRDF_H

#include "common.h"
#include "helpers.h"

namespace BRDF {
    // Fresnel term
    Colour FresnelSchlick(const Vec3& H, const Vec3& V, const Colour& F0);

    // Normal Distribution Function (GGX)
    float GGX_D(const Vec3& N, const Vec3& H, float roughness);

    float GGX_PDF(const Vec3& N, const Vec3& H, float roughness);

    // Geometry Function (Schlick-GGX)
    float SchlickG(const Vec3& N, const Vec3& V, float roughness);

    // Cook-Torrance BRDF
    Colour CookTorrance(
        const Vec3& lightDir,
        const Vec3& viewDir,
        const Vec3& normal,
        const Vec3& halfVector,
        const Colour& baseColor,
        const Colour& F0,
        float roughness
    );

    Vec3 sampleGGX(const Vec3& normal, float roughness);

    Vec3 toWorld(const Vec3& local, const Vec3& normal);
}

#endif
