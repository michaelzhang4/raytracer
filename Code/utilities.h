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

    Vec3 operator+(const Vec3& other) const;
    Vec3 operator-() const {
        return Vec3(-x, -y, -z);
    }
    Vec3 operator-(const Vec3& other) const;
    
    Vec3 operator/(float scalar) const;
    Vec3 operator*(float scalar) const;
    Vec3 cross(const Vec3& other) const;
    Vec3 normalise() const;
    float dot(const Vec3& other) const;
};

struct Colour {
    int r, g, b;

    // Constructor
    Colour(int red = 0, int green = 0, int blue = 0);

    // Method to clamp colour values
    void clamp();

    // Operator overloading (prototypes)
    Colour operator*(float scalar) const;
    Colour operator+(const Colour& other) const;
};


#endif
