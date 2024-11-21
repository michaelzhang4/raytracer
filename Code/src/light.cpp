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