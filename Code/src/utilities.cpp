#include "utilities.h"

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
        static_cast<int>(r / scalar),
        static_cast<int>(g / scalar),
        static_cast<int>(b / scalar)
    );
}

Ray::Ray(const Vec3& origin, const Vec3& direction)
    : origin(origin), direction(direction.normalise()) {}

Vec3 Ray::at(float t) const {
    return origin + direction * t;
}

// Constructor
BoundingBox::BoundingBox(const Vec3& min, const Vec3& max) : min(min), max(max) {}

// Check if a point is inside the bounding box
bool BoundingBox::contains(const Vec3& point) const {
    return (point.x >= min.x && point.x <= max.x &&
            point.y >= min.y && point.y <= max.y &&
            point.z >= min.z && point.z <= max.z);
}

// Check if this bounding box intersects with another
bool BoundingBox::intersects(const BoundingBox& other) const {
    return (min.x <= other.max.x && max.x >= other.min.x &&
            min.y <= other.max.y && max.y >= other.min.y &&
            min.z <= other.max.z && max.z >= other.min.z);
}

// Expand the bounding box to include another point
void BoundingBox::expand(const Vec3& point) {
    min.x = std::min(min.x, point.x);
    min.y = std::min(min.y, point.y);
    min.z = std::min(min.z, point.z);
    max.x = std::max(max.x, point.x);
    max.y = std::max(max.y, point.y);
    max.z = std::max(max.z, point.z);
}

// Merge this bounding box with another to create a new bounding box
BoundingBox BoundingBox::merge(const BoundingBox& box1, const BoundingBox& box2) {
    Vec3 newMin(
        std::min(box1.min.x, box2.min.x),
        std::min(box1.min.y, box2.min.y),
        std::min(box1.min.z, box2.min.z)
    );

    Vec3 newMax(
        std::max(box1.max.x, box2.max.x),
        std::max(box1.max.y, box2.max.y),
        std::max(box1.max.z, box2.max.z)
    );

    return BoundingBox(newMin, newMax);
}
