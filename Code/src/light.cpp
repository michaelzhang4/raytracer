#include "light.h"

// Base Light Implementation
Light::Light(const Vec3& position, const Colour& intensity) 
    : position(position), intensity(intensity) {}

// Point Light Implementation
PointLight::PointLight(const Vec3& position, const Colour& intensity) 
    : Light(position, intensity) {}

// Sample a point on the light (Point light always returns its position)
Vec3 PointLight::samplePoint() const {
    return position;
}

// PDF is always 1.0 for a point light
float PointLight::pdf() const {
    return 1.0f;
}

// Compute illumination from the point light
Colour PointLight::computeIllumination(const Vec3& point, const Vec3& normal, const Vec3& viewDir) const {
    Vec3 lightDir = (position - point).normalise();

    // Diffuse contribution
    float diffuseFactor = std::max(0.0f, normal.dot(lightDir));
    Colour diffuse = intensity * diffuseFactor;

    // Specular contribution (Blinn-Phong example)
    Vec3 halfVector = (lightDir + viewDir).normalise();
    float specularFactor = std::pow(std::max(0.0f, normal.dot(halfVector)), 16.0f);
    Colour specular = intensity * specularFactor;

    // Return combined contribution
    return diffuse + specular;
}

// Area Light Implementation
AreaLight::AreaLight(const Vec3& position, const Colour& intensity, const Vec3& u, const Vec3& v, float width, float height)
    : Light(position, intensity), u(u), v(v), width(width), height(height) {}

Vec3 AreaLight::samplePoint() const {
    // Ensure tangent vectors u and v are orthogonal
    if (std::abs(u.dot(v)) > 1e-6) {
        std::cerr << "Warning: Tangent vectors u and v are not orthogonal!" << std::endl;
    }

    // Random sampling within the light's area
    float randU = generateRandomNumber(0.0f, 1.0f); // Uniform [0,1]
    float randV = generateRandomNumber(0.0f, 1.0f); // Uniform [0,1]
    return position + u * (randU - 0.5f) * width + v * (randV - 0.5f) * height;
}

float AreaLight::pdf() const {
    if (width <= 0 || height <= 0) {
        std::cerr << "Error: AreaLight dimensions are invalid!" << std::endl;
        return 1.0f; // Default fallback
    }
    return 1.0f / (width * height);
}

Colour AreaLight::computeIllumination(const Vec3& point, const Vec3& normal, const Vec3& viewDir) const {
    Vec3 sample = samplePoint();
    Vec3 lightDir = (sample - point).normalise();
    float distanceSquared = (sample - point).length() * (sample - point).length();

    // Diffuse contribution
    float diffuseFactor = std::max(0.0f, normal.dot(lightDir));
    Colour diffuse = intensity * diffuseFactor / distanceSquared;

    // Specular contribution (Blinn-Phong)
    Vec3 halfVector = (lightDir + viewDir).normalise();
    float shininess = 16.0f; // Replace with a parameter if needed
    float specularFactor = std::pow(std::max(0.0f, normal.dot(halfVector)), shininess);
    Colour specular = intensity * specularFactor / distanceSquared;

    // Return combined contribution, scaled by area size
    return (diffuse + specular) / (width * height);
}
