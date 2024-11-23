#include "brdf.h"

// Fresnel term (Schlick's approximation)
Colour BRDF::FresnelSchlick(const Vec3& H, const Vec3& V, const Colour& F0) {
    float cosTheta = std::max(H.normalise().dot(V.normalise()), 0.0f);
    Colour fresnel = F0 + (Colour(1.0f, 1.0f, 1.0f) - F0) * std::pow(1.0f - cosTheta, 5.0f);
    return fresnel;
}

// GGX Normal Distribution Function
float BRDF::GGX_D(const Vec3& N, const Vec3& H, float roughness) {
    float alpha = roughness * roughness;
    float nh = std::max(N.dot(H), 0.0f);
    float denom = nh * nh * (alpha * alpha - 1.0f) + 1.0f;
    float D = (alpha * alpha) / (M_PI * denom * denom);
    return std::clamp(D, 0.0f, 1.0f); 
}

float BRDF::GGX_PDF(const Vec3& N, const Vec3& H, float roughness) {
    // Use GGX normal distribution function (D) to calculate PDF
    float D = GGX_D(N, H, roughness);  // Normal distribution function
    float nh = std::max(N.dot(H), 0.0f);  // |N . H|
    float pdf = (D * nh) / std::max(4.0f * std::abs(nh), 0.001f);  // Prevent division by zero
    return std::clamp(pdf, 0.0f, 1.0f);
}

// Schlick-GGX Geometry Function
float BRDF::SchlickG(const Vec3& N, const Vec3& V, float roughness) {
    float alpha = roughness * roughness;
    float k = alpha / 2.0f;
    float nv = std::max(N.dot(V), 0.0f);
    return nv / (nv * (1.0f - k) + k);
}


// Cook-Torrance BRDF
Colour BRDF::CookTorrance(
    const Vec3& lightDir,
    const Vec3& viewDir,
    const Vec3& normal,
    const Vec3& halfVector,
    const Colour& baseColor,
    const Colour& F0,
    float roughness
) {
    // Normalize vectors
    Vec3 N = normal.normalise();
    Vec3 L = lightDir.normalise();
    Vec3 V = viewDir.normalise();
    Vec3 H = halfVector.normalise();

    // Ensure roughness is within valid range
    float alpha = std::max(roughness * roughness, 0.05f);

    // GGX Normal Distribution Function
    float D = GGX_D(N, H, alpha);

    // Geometry Function
    float k = (roughness + 1.0f) * (roughness + 1.0f) / 8.0f;  // For Schlick's G
    float G = SchlickG(N, V, k) * SchlickG(N, L, k);

    // Fresnel Term
    Colour F = FresnelSchlick(H, V, F0);

    // Denominator
    float NdotL = std::max(N.dot(L), 0.0f);
    float NdotV = std::max(N.dot(V), 0.0f);

    // If NdotL or NdotV are zero, no light is reflected
    if (NdotL <= 0.0f || NdotV <= 0.0f) {
        return Colour(0.0f, 0.0f, 0.0f);
    }

    float denominator = 4.0f * NdotL * NdotV + 1e-7f;  // Add epsilon to prevent division by zero

    // Cook-Torrance Equation
    Colour specular = (F * D * G) / denominator;

    // Lambertian Diffuse
    Colour diffuse = baseColor * (1.0f / M_PI) * NdotL;

    // Final color
    return diffuse + specular;
}



Vec3 BRDF::sampleGGX(const Vec3& normal, float roughness) {
    // Generate random numbers for sampling
    float xi1 = generateRandomNumber(0.0f, 1.0f); // Uniform random number [0, 1]
    float xi2 = generateRandomNumber(0.0f, 1.0f);

    // Convert roughness to alpha
    float alpha = roughness * roughness;

    // Compute theta and phi using GGX sampling
    float cosTheta = std::sqrt((1.0f - xi1) / (1.0f + (alpha * alpha - 1.0f) * xi1));
    float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);
    float phi = 2.0f * M_PI * xi2;

    // Convert spherical coordinates to Cartesian coordinates
    float x = sinTheta * std::cos(phi);
    float y = sinTheta * std::sin(phi);
    float z = cosTheta;

    // Transform sampled vector to the local tangent space of the surface
    Vec3 hLocal(x, y, z);
    return toWorld(hLocal, normal);
}


Vec3 BRDF::toWorld(const Vec3& local, const Vec3& normal) {
    // Construct an orthonormal basis around the surface normal
    Vec3 tangent = (std::abs(normal.x) > 0.99f ? Vec3(0.0f, 1.0f, 0.0f) : Vec3(1.0f, 0.0f, 0.0f)).cross(normal).normalise();
    Vec3 bitangent = normal.cross(tangent);
    return (tangent * local.x + bitangent * local.y + normal * local.z).normalise();
}
