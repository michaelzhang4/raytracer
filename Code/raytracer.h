#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <string>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <memory>

#include "parser.h"

// Define M_PI if not defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// RGB Colours
struct Colour {
    int r, g, b;
    Colour(int red = 0, int green = 0, int blue = 0) : r(red), g(green), b(blue) {}
};

// 3D vector coordinates
struct Vec3 {
    float x, y, z;
    // Overloading for operators
    Vec3 operator+(const Vec3& other) const;
    Vec3 operator-(const Vec3& other) const;
    Vec3 operator*(float scalar) const;
    Vec3 cross(const Vec3& other) const;
    Vec3 normalize() const;
    float dot(const Vec3& other) const;
};

// Camera class
class Camera {
public:
    Vec3 position;
    Vec3 lookAt;
    Vec3 upVector;
    int width;
    int height;
    float fov;
    float exposure;
    float aspectRatio;
    // Default constructor
    Camera() 
        : position({0.0f, 0.0f, 0.0f}), lookAt({0.0f, 0.0f, -1.0f}), upVector({0.0f, 1.0f, 0.0f}),
          width(800), height(600), fov(90.0f), exposure(1.0f), aspectRatio(4.0f / 3.0f) {}

    // Parameterised constructor
    Camera(const Vec3& pos, const Vec3& look, const Vec3& upVec, int w, int h, float fieldOfView, float expos, float aspect);
    Vec3 getRayDirection(int pixelX, int pixelY) const;
    void printCameraSpecs();
};

// Abstract Shape class
class Shape {
public:
    virtual ~Shape() = default;  // Virtual destructor for polymorphic deletion
    virtual bool intersect(const Vec3& rayOrigin, const Vec3& rayDir) const = 0;  // Pure virtual method
    virtual void printInfo() const = 0;
};

// Sphere class
class Sphere : public Shape {
public:
    Vec3 center;
    float radius;
    Sphere(const Vec3& centerPos, float rad);
    bool intersect(const Vec3& rayOrigin, const Vec3& rayDir) const override;
    void printInfo() const override;
};

// Cylinder class
class Cylinder : public Shape {
public:
    Vec3 center;
    Vec3 axis;
    float radius;
    float height;
    Cylinder(const Vec3& centerPos, const Vec3& ax, float rad, float h);
    bool intersect(const Vec3& rayOrigin, const Vec3& rayDir) const override;
    void printInfo() const override;
};

// Triangle class
class Triangle : public Shape {
public:
    Vec3 v0, v1, v2;
    Triangle(const Vec3& vertex0, const Vec3& vertex1, const Vec3& vertex2);
    bool intersect(const Vec3& rayOrigin, const Vec3& rayDir) const override;
    void printInfo() const override;
};

// Function declarations
Colour calculateColour(const Vec3& rayDirection);
void renderScene(std::vector<Colour>& pixels, int width, int height, const Camera& camera);
void writePPM(const std::string &filename, const std::vector<Colour> &pixels, int width, int height);

// Scene class
class Scene {
public:
    Scene(const json& jsonData);
    void renderScene(std::vector<Colour>& pixels);
    void printSceneInfo();
private:
    Camera camera;
    Colour backgroundColour;
    std::vector<std::shared_ptr<Shape>> shapes;
};

#endif // RAYTRACER_H
