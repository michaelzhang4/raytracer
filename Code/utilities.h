#ifndef UTILITIES_H
#define UTILITIES_H

#include <cmath> 

enum class RenderMode {
    BINARY,
    PHONG
};

struct Vec3 {
    float x, y, z;
    Vec3 operator+(const Vec3& other) const;
    Vec3 operator-(const Vec3& other) const;
    Vec3 operator*(float scalar) const;
    Vec3 cross(const Vec3& other) const;
    Vec3 normalise() const;
    float dot(const Vec3& other) const;
};

// RGB Colours
struct Colour {
    int r, g, b;
    Colour(int red = 0, int green = 0, int blue = 0) : r(red), g(green), b(blue) {}
};


#endif
