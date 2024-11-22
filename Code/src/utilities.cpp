#include "utilities.h"
#include <stdexcept>

Vec3 Vec3::min(const Vec3& other) const {
    return {std::min(x,other.x), std::min(y,other.y), std::min(z,other.z)};
}

Vec3 Vec3::max(const Vec3& other) const {
    return {std::max(x,other.x), std::max(y,other.y), std::max(z,other.z)};
}

// Vec3 method definitions
Vec3 Vec3::operator+(const Vec3& other) const {
    return {x + other.x, y + other.y, z + other.z};
}

Vec3 Vec3::operator+(float scalar) const {
    return {x + scalar, y + scalar, z + scalar};
}

Vec3 Vec3::operator-(const Vec3& other) const {
    return {x - other.x, y - other.y, z - other.z};
}

Vec3 Vec3::operator*(float scalar) const {
    return {x * scalar, y * scalar, z * scalar};
}

Vec3 Vec3::operator*(const Vec3& other) const {
    return {x * other.x, y * other.y, z * other.z};
}

Vec3 Vec3::operator/(float scalar) const {
    if (scalar != 0.0f) {
        return {x / scalar, y / scalar, z / scalar};
    }
    return {0.0f, 0.0f, 0.0f};  // Return zero vector on division by zero
}

Vec3 Vec3::operator/(const Vec3& other) const {
    return {
        other.x != 0.0f ? x / other.x : 0.0f,
        other.y != 0.0f ? y / other.y : 0.0f,
        other.z != 0.0f ? z / other.z : 0.0f
    };
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

// Length method
float Vec3::length() const {
    return std::sqrt(x * x + y * y + z * z);
}

// Const version (read-only access)
const float& Vec3::operator[](int index) const {
    if (index == 0) return x;
    if (index == 1) return y;
    if (index == 2) return z;

    throw std::out_of_range("Index out of range");
}

// Non-const version (allows modification)
float& Vec3::operator[](int index) {
    if (index == 0) return x;
    if (index == 1) return y;
    if (index == 2) return z;

    throw std::out_of_range("Index out of range");
}

// Constructor
Colour::Colour(float red, float green, float blue) : r(red), g(green), b(blue) {}
Colour::Colour(int red, int green, int blue)
    : r(static_cast<float>(red)), g(static_cast<float>(green)), b(static_cast<float>(blue)) {}
    
// Method to clamp colour values to the range [0, 255]
void Colour::clamp() {
    r = std::clamp(r, 0.0f, 255.0f);
    g = std::clamp(g, 0.0f, 255.0f);
    b = std::clamp(b, 0.0f, 255.0f);
}
// Operator overloading: multiply by scalar
Colour Colour::operator*(float scalar) const {
    return Colour(
        r * scalar,
        g * scalar,
        b * scalar
    );
}

// Operator overloading: addition
Colour Colour::operator+(const Colour& other) const {
    return Colour(r + other.r, g + other.g, b + other.b);
}

// Component-wise multiplication of two Colour objects
Colour Colour::operator*(const Colour& other) const {
    return Colour(
        static_cast<int>(r * (other.r / 255.0f)),  // Normalize to 0-1 range for blending
        static_cast<int>(g * (other.g / 255.0f)),
        static_cast<int>(b * (other.b / 255.0f))
    );
}

// Overload division operator for scalar
Colour Colour::operator/(float scalar) const {
    return Colour(
        r / scalar,
        g / scalar,
        b / scalar
    );
}

Colour Colour::operator-(const Colour& other) const {
    return Colour(r - other.r, g - other.g, b - other.b);
}

bool Colour::belowThreshold() const {
    const float threshold = 0.1f;
    return r <= threshold && g <= threshold && b <= threshold;
}

Ray::Ray(const Vec3& origin, const Vec3& direction)
    : origin(origin), direction(direction.normalise()) {}

Vec3 Ray::at(float t) const {
    return origin + direction * t;
}