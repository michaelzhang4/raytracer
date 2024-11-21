#ifndef LIGHT_H
#define LIGHT_H

#include "utilities.h"
#include "helpers.h"


class Light {
public:
    Vec3 position;     // Light position
    Colour intensity;  // RGB intensity of the light

    // Constructor
    Light(const Vec3& position, const Colour& intensity);

    // Virtual destructor for inheritance
    virtual ~Light() = default;

    // Pure virtual methods for sampling and PDF
    virtual Vec3 samplePoint() const = 0; // Sample a point on the light
    virtual float pdf() const = 0; // PDF for light sampling
    virtual Colour computeIllumination(const Vec3& point, const Vec3& normal, const Vec3& viewDir) const = 0;
};

// Point Light Class (Derived from Light)
class PointLight : public Light {
public:
    // Constructor
    PointLight(const Vec3& position, const Colour& intensity);

    // Override base class methods
    Vec3 samplePoint() const override;
    float pdf() const override;
    Colour computeIllumination(const Vec3& point, const Vec3& normal, const Vec3& viewDir) const override;
};

// Area Light Class (Derived from Light)
class AreaLight : public Light {
public:
    Vec3 u, v;          // Tangent vectors defining the light's plane
    float width, height; // Dimensions of the area light

    // Constructor
    AreaLight(const Vec3& position, const Colour& intensity, const Vec3& u, const Vec3& v, float width, float height);

    // Override base class methods
    Vec3 samplePoint() const override;
    float pdf() const override;
    Colour computeIllumination(const Vec3& point, const Vec3& normal, const Vec3& viewDir) const override;
};


#endif // LIGHT_H