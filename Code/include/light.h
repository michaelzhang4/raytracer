#ifndef LIGHT_H
#define LIGHT_H

#include "utilities.h"
#include "helpers.h"
#include "material.h"

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
};

// Point Light Class (Derived from Light)
class PointLight : public Light {
public:
    // Constructor
    PointLight(const Vec3& position, const Colour& intensity);

    // Override base class methods
    Vec3 samplePoint() const override;
    float pdf() const override;
};

// Area Light Class (Derived from Light)
class AreaLight : public Light {
public:
    Vec3 u, v;          // Tangent vectors defining the light's plane
    float width, height; // Dimensions of the area light
    Colour radiance;

    // Constructor
    AreaLight(const Vec3& position, const Colour& intensity, const Vec3& u, const Vec3& v, float width, float height, Colour radiance);
    Vec3 getNormal() const;
    // Override base class methods
    Vec3 samplePoint() const override;
    float pdf() const override;
};


#endif // LIGHT_H