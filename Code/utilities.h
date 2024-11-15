#ifndef UTILITIES_H
#define UTILITIES_H

#include <cmath>
#include <algorithm>

enum class RenderMode {
    BINARY,
    PHONG,
    PATH
};

struct Vec3 {
    float x, y, z;

    // Default constructor
    Vec3() : x(0.0f), y(0.0f), z(0.0f) {}

    // Constructor with three components
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    // Constructor to initialise all components to the same scalar value
    explicit Vec3(float scalar) : x(scalar), y(scalar), z(scalar) {}

    Vec3 operator+(const Vec3& other) const;
    Vec3 operator+(float scalar) const;
    Vec3 operator-() const {
        return Vec3(-x, -y, -z);
    }
    Vec3 operator-(const Vec3& other) const;
    
    Vec3 operator/(float scalar) const;
    Vec3 operator/(const Vec3& other) const;  // Component-wise division
    Vec3 operator*(float scalar) const;
    Vec3 operator*(const Vec3& other) const;
    Vec3 cross(const Vec3& other) const;
    Vec3 normalise() const;
    float length() const;
    float dot(const Vec3& other) const;
};

struct Colour {
    int r, g, b;

    // Constructor
    Colour(int red = 0, int green = 0, int blue = 0);

    // Method to clamp colour values
    void clamp();

    Vec3 toVec3() const {
        return Vec3(r / 255.0f, g / 255.0f, b / 255.0f);
    }
    // Operator overloading (prototypes)
    Colour operator*(float scalar) const;
    Colour operator*(const Colour& other) const;
    Colour operator+(const Colour& other) const;
    Colour operator/(float scalar) const;
};

class BoundingBox {
public:
    Vec3 min;  // Minimum corner of the bounding box
    Vec3 max;  // Maximum corner of the bounding box

    // Constructor
    BoundingBox(const Vec3& min, const Vec3& max);

    // Check if a point is inside the bounding box
    bool contains(const Vec3& point) const;

    // Check if this bounding box intersects with another
    bool intersects(const BoundingBox& other) const;

    // Expand the bounding box to include another point
    void expand(const Vec3& point);

    // Merge this bounding box with another to create a new bounding box
    static BoundingBox merge(const BoundingBox& box1, const BoundingBox& box2);
};


#endif
