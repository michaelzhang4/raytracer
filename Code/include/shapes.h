#ifndef SHAPE_H
#define SHAPE_H

#include "common.h"
#include "material.h"

class Shape; // Forward declaration for Shape

struct Intersection {
    bool hit;                   // Did the ray intersect the object?
    float t;                    // Distance to the intersection point
    Vec3 hitPoint;              // The exact point of intersection
    Vec3 normal;                // Surface normal at the intersection
    const Shape* shape;
};

class Shape {
protected:
    std::shared_ptr<Material> material;
public:
    Shape(std::shared_ptr<Material> mat) : material(std::move(mat)) {}
    // Getter for material
    std::shared_ptr<Material> getMaterial() const {
        return material;
    }
    virtual ~Shape() = default;
    virtual std::pair<float, float> getUV(const Vec3& hitPoint) const = 0;
    virtual bool intersect(const Ray& ray, Intersection& intersection) const = 0;
    virtual void printInfo() const = 0;
    virtual Vec3 getNormal(const Vec3& hitPoint) const = 0;

};

// Sphere class
class Sphere : public Shape {
public:
    Vec3 center;
    float radius;

    Sphere(const Vec3& centerPos, float rad, std::shared_ptr<Material> mat);
    Vec3 getNormal(const Vec3& hitPoint) const override;
    bool intersect(const Ray& ray, Intersection& intersection) const override;
    std::pair<float, float> getUV(const Vec3& hitPoint) const override;
    void printInfo() const override;
};

// Cylinder class
class Cylinder : public Shape {
public:
    Vec3 center;
    Vec3 axis;
    float radius;
    float height;
    Cylinder(const Vec3& centerPos, const Vec3& ax, float rad, float h, std::shared_ptr<Material> mat);
    Vec3 getNormal(const Vec3& hitPoint) const override;
    bool intersect(const Ray& ray, Intersection& intersection) const;
    std::pair<float, float> getUV(const Vec3& hitPoint) const override;
    BoundingBox getBoundingBox() const;
    void printInfo() const override;
};

// Triangle class
class Triangle : public Shape {
public:
    Vec3 v0, v1, v2;
    std::pair<float, float> uv0;
    std::pair<float, float> uv1;
    std::pair<float, float> uv2;
    Triangle(const Vec3& vertex0, const Vec3& vertex1, const Vec3& vertex2, 
             const std::pair<float, float>& uv0,
             const std::pair<float, float>& uv1,
             const std::pair<float, float>& uv2,
             std::shared_ptr<Material> mat);
    Vec3 getNormal(const Vec3& hitPoint) const override;
    bool intersect(const Ray& ray, Intersection& intersection) const override;
    std::pair<float, float> getUV(const Vec3& hitPoint) const override;
    void printInfo() const override;
};

Vec3 computeBarycentricCoordinates(const Vec3& p, const Vec3& a, const Vec3& b, const Vec3& c);

#endif // SHAPE_H
