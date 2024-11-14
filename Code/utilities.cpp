#include "utilities.h"

// Vec3 method definitions
Vec3 Vec3::operator+(const Vec3& other) const {
    return {x + other.x, y + other.y, z + other.z};
}

Vec3 Vec3::operator-(const Vec3& other) const {
    return {x - other.x, y - other.y, z - other.z};
}

Vec3 Vec3::operator*(float scalar) const {
    return {x * scalar, y * scalar, z * scalar};
}

Vec3 Vec3::operator/(float scalar) const {
    if (scalar != 0.0f) {
        return {x / scalar, y / scalar, z / scalar};
    }
    return {0.0f, 0.0f, 0.0f};  // Return zero vector on division by zero
}

Vec3 Vec3::cross(const Vec3& other) const {
    return {y * other.z - z * other.y, 
            z * other.x - x * other.z, 
            x * other.y - y * other.x};
}

Vec3 Vec3::normalise() const {
    float length = std::sqrt(x * x + y * y + z * z);
    if (length > 0.0f) {
        return *this / length;  // Use operator/
    }
    return {0.0f, 0.0f, 0.0f};  // Return zero vector if length is zero
}

float Vec3::dot(const Vec3& other) const {
    return x * other.x + y * other.y + z * other.z;
}

// Constructor
Colour::Colour(int red, int green, int blue) : r(red), g(green), b(blue) {}

// Method to clamp colour values to the range [0, 255]
void Colour::clamp() {
    r = std::clamp(r, 0, 255);
    g = std::clamp(g, 0, 255);
    b = std::clamp(b, 0, 255);
}

// Operator overloading: multiply by scalar
Colour Colour::operator*(float scalar) const {
    return Colour(
        static_cast<int>(r * scalar),
        static_cast<int>(g * scalar),
        static_cast<int>(b * scalar)
    );
}

// Operator overloading: addition
Colour Colour::operator+(const Colour& other) const {
    return Colour(r + other.r, g + other.g, b + other.b);
}