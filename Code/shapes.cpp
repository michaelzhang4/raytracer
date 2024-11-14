#include "shapes.h"

// Sphere class definition
Sphere::Sphere(const Vec3 &centerPos, float rad, const Material& mat)
    : Shape(mat), center(centerPos), radius(rad) {}

Vec3 Sphere::getNormal(const Vec3& hitPoint) const {
    return (hitPoint - center).normalise();
}

bool Sphere::intersect(const Vec3& rayOrigin, const Vec3& rayDir, float& t) const {
    Vec3 oc = rayOrigin - center;
    float a = rayDir.dot(rayDir);
    float b = 2.0f * oc.dot(rayDir);
    float c = oc.dot(oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return false;
    }

    float sqrtDiscriminant = std::sqrt(discriminant);
    float t1 = (-b - sqrtDiscriminant) / (2.0f * a);
    float t2 = (-b + sqrtDiscriminant) / (2.0f * a);

    if (t1 > EPSILON) {
        t = t1;
    } else if (t2 > EPSILON) {
        t = t2;
    } else {
        return false;
    }

    return true;
}

void Sphere::printInfo() const {
    std::cout << "Sphere Info:" << std::endl;
    std::cout << "center (x,y,z): " << center.x << " " << center.y << " " << center.z << std::endl;
    std::cout << "radius: " << radius << std::endl;
    material.printMaterialInfo();
}

// Cylinder class definition
Cylinder::Cylinder(const Vec3& centerPos, const Vec3& ax, float rad, float h, const Material& mat)
    : Shape(mat), center(centerPos), axis(ax.normalise()), radius(rad), height(h) {}

Vec3 Cylinder::getNormal(const Vec3& hitPoint) const {
    Vec3 projection = axis * ((hitPoint - center).dot(axis));
    return ((hitPoint - center) - projection).normalise();
}

bool Cylinder::intersect(const Vec3& rayOrigin, const Vec3& rayDir, float& t) const {
    Vec3 oc = rayOrigin - center;
    float dDotA = rayDir.dot(axis);
    float ocDotA = oc.dot(axis);

    Vec3 dPerp = rayDir - axis * dDotA;
    Vec3 ocPerp = oc - axis * ocDotA;

    float a = dPerp.dot(dPerp);
    float b = 2.0f * ocPerp.dot(dPerp);
    float c = ocPerp.dot(ocPerp) - radius * radius;

    float discriminant = b * b - 4.0f * a * c;

    if (discriminant < 0) {
        return false;
    }

    float sqrtDiscriminant = std::sqrt(discriminant);
    float t1 = (-b - sqrtDiscriminant) / (2.0f * a);
    float t2 = (-b + sqrtDiscriminant) / (2.0f * a);

    if (t1 > EPSILON && (t2 < EPSILON || t1 < t2)) {
        t = t1;
    } else if (t2 > EPSILON) {
        t = t2;
    } else {
        return false;
    }

    Vec3 p = rayOrigin + rayDir * t;
    float heightAtP = (p - center).dot(axis);

    if (heightAtP < 0 || heightAtP > height) {
        return false;
    }

    return true;
}

void Cylinder::printInfo() const {
    std::cout << "Cylinder Info:" << std::endl;
    std::cout << "center (x,y,z): " << center.x << " " << center.y << " " << center.z << std::endl;
    std::cout << "axis (x,y,z): " << axis.x << " " << axis.y << " " << axis.z << std::endl;
    std::cout << "radius: " << radius << std::endl;
    std::cout << "height: " << height << std::endl;
    material.printMaterialInfo();
}

// Triangle class definition
Triangle::Triangle(const Vec3& vertex0, const Vec3& vertex1, const Vec3& vertex2, const Material& mat)
    : Shape(mat), v0(vertex0), v1(vertex1), v2(vertex2) {}

Vec3 Triangle::getNormal(const Vec3& hitPoint) const {
    Vec3 edge1 = v1 - v0;
    Vec3 edge2 = v2 - v0;
    return edge1.cross(edge2).normalise();
}

bool Triangle::intersect(const Vec3& rayOrigin, const Vec3& rayDir, float& t) const {
    Vec3 edge1 = v1 - v0;
    Vec3 edge2 = v2 - v0;

    Vec3 h = rayDir.cross(edge2);
    float a = edge1.dot(h);

    if (std::abs(a) < EPSILON) {
        return false;
    }

    float f = 1.0f / a;
    Vec3 s = rayOrigin - v0;
    float u = f * s.dot(h);

    if (u < 0.0f || u > 1.0f) {
        return false;
    }

    Vec3 q = s.cross(edge1);
    float v = f * rayDir.dot(q);

    if (v < 0.0f || u + v > 1.0f) {
        return false;
    }

    t = f * edge2.dot(q);

    if (t > EPSILON) {
        return true;
    }

    return false;
}

void Triangle::printInfo() const {
    std::cout << "Triangle Info:" << std::endl;
    std::cout << "v0 (x,y,z): " << v0.x << " " << v0.y << " " << v0.z << std::endl;
    std::cout << "v1 (x,y,z): " << v1.x << " " << v1.y << " " << v1.z << std::endl;
    std::cout << "v2 (x,y,z): " << v2.x << " " << v2.y << " " << v2.z << std::endl;
    material.printMaterialInfo();
}
