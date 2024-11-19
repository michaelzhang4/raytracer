#include "ray.h"

Ray::Ray(const Vec3& origin, const Vec3& direction)
    : origin(origin), direction(direction.normalise()) {}

Vec3 Ray::at(float t) const {
    return origin + direction * t;
}
