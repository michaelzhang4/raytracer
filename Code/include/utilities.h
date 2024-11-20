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

    Vec3 min(const Vec3& other) const;
    Vec3 max(const Vec3& other) const;
    // Access operator (const version for read-only access)
    const float& operator[](int index) const;

    // Access operator (non-const version for modification)
    float& operator[](int index);
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

struct Light {
    Vec3 position;
    Colour intensity;  // RGB intensity of the light
    Light(const Vec3& pos, const Colour& intensity) : position(pos), intensity(intensity) {}
    float kc = 1.0f;   // Constant attenuation factor
    float kl = 0.1f;   // Linear attenuation factor
    float kq = 0.01f;  // Quadratic attenuation factor
};

class Ray {
public:
    Vec3 origin;
    Vec3 direction;

    Ray(const Vec3& origin, const Vec3& direction);

    Vec3 at(float t) const;
};

#endif
