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

// BinaryTracer for simple hit testing
class BinaryTracer : public RayTracer {
public:
    void renderScene(const Scene& scene, std::vector<Colour>& pixels) const override;
};

// PhongTracer for basic Phong shading
class PhongTracer : public RayTracer {
public:
    void renderScene(const Scene& scene, std::vector<Colour>& pixels) const override;
};

// // PathTracer for global illumination
// class PathTracer : public RayTracer {
// public:
//     void renderScene(const Scene& scene, std::vector<Colour>& pixels) const override {
//         // Implement path tracing logic here
//         std::cout << "Rendering with PathTracer..." << std::endl;
//     }

//     void printTracerInfo() const override {
//         std::cout << "Path Tracer (Global Illumination)" << std::endl;
//     }
// };

#endif // RAYTRACER_H
