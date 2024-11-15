#ifndef SHAPE_H
#define SHAPE_H

#include "common.h"
#include "material.h"

struct Intersection {
    bool hit;                   // Did the ray intersect the object?
    float t;                    // Distance to the intersection point
    Vec3 hitPoint;              // The exact point of intersection
    Vec3 normal;                // Surface normal at the intersection
    Material material;          // Material properties of the hit object
};

class Shape {
public:
    Material material;
    Shape(const Material& mat) : material(mat) {}
    virtual ~Shape() = default;

    virtual bool intersect(const Vec3& rayOrigin, const Vec3& rayDir, Intersection& intersection) const = 0;
    virtual void printInfo() const = 0;
    virtual Vec3 getNormal(const Vec3& hitPoint) const = 0;

};

// Sphere class
class Sphere : public Shape {
public:
    Vec3 center;
    float radius;

    Sphere(const Vec3& centerPos, float rad, const Material& mat);
    Vec3 getNormal(const Vec3& hitPoint) const override;
    bool intersect(const Vec3& rayOrigin, const Vec3& rayDir, Intersection& intersection) const override;
    void printInfo() const override;
};

// Cylinder class
class Cylinder : public Shape {
public:
    Vec3 center;
    Vec3 axis;
    float radius;
    float height;
    Cylinder(const Vec3& centerPos, const Vec3& ax, float rad, float h, const Material& mat);
    Vec3 getNormal(const Vec3& hitPoint) const override;
    bool intersect(const Vec3& rayOrigin, const Vec3& rayDir, Intersection& intersection) const;
    BoundingBox getBoundingBox() const;
    void printInfo() const override;
};

// Triangle class
class Triangle : public Shape {
public:
    Vec3 v0, v1, v2;
    Triangle(const Vec3& vertex0, const Vec3& vertex1, const Vec3& vertex2, const Material& mat);
    Vec3 getNormal(const Vec3& hitPoint) const override;
    bool intersect(const Vec3& rayOrigin, const Vec3& rayDir, Intersection& intersection) const override;
    void printInfo() const override;
};

#endif // SHAPE_H
