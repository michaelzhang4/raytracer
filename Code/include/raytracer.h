#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "common.h"

class Scene;

// Base RayTracer class
class RayTracer {
public:
    virtual ~RayTracer() = default;

    // Pure virtual function for rendering the scene
    virtual void renderScene(const Scene& scene, std::vector<Colour>& pixels) const = 0;
};

// BinaryTracer
class BinaryTracer : public RayTracer {
public:
    void renderScene(const Scene& scene, std::vector<Colour>& pixels) const override;
};

// PhongTracer for basic Phong shading
class PhongTracer : public RayTracer {
public:
    void renderScene(const Scene& scene, std::vector<Colour>& pixels) const override;
    Colour traceRayRecursive(const Scene& scene, const Ray& ray, int bounce) const;
};

// PathTracer for advanced features
class PathTracer : public RayTracer {
public:
    void renderScene(const Scene& scene, std::vector<Colour>& pixels) const override;
    Colour tracePixel(const Scene& scene, int x, int y) const;
    Colour traceRayRecursive(const Scene& scene, const Ray& ray, int bounce) const;

};

float fresnel(const Vec3& I, const Vec3& N, float eta);
float generateRandomNumber(float min, float max);

#endif // RAYTRACER_H
