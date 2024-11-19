#include "shapes.h"

// Sphere class definition
Sphere::Sphere(const Vec3 &centerPos, float rad, const Material& mat)
    : Shape(mat), center(centerPos), radius(rad) {}

Vec3 Sphere::getNormal(const Vec3& hitPoint) const {
    return (hitPoint - center).normalise();
}

bool Sphere::intersect(const Vec3& rayOrigin, const Vec3& rayDir, Intersection& intersection) const {
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

    float t = (t1 > EPSILON) ? t1 : (t2 > EPSILON ? t2 : -1.0f);
    if (t < 0) {
        return false;
    }

    // Populate the Intersection struct
    intersection.hit = true;
    intersection.t = t;
    intersection.hitPoint = rayOrigin + rayDir * t;
    intersection.normal = getNormal(intersection.hitPoint);
    intersection.material = material;

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

// Vec3 Cylinder::getNormal(const Vec3& hitPoint) const {
//     Vec3 toHit = hitPoint - center;
//     float heightAtP = toHit.dot(axis); 

//     if (std::abs(heightAtP + height) < EPSILON) {
//         return -axis; // Bottom cap
//     }
    
//     if (std::abs(heightAtP - height) < EPSILON) {
//         return axis; // Top cap
//     }

//     // Surface normal
//     Vec3 projection = axis * heightAtP;
//     return (toHit - projection).normalise();
// }

Vec3 Cylinder::getNormal(const Vec3& point) const {
    Vec3 axisNorm = axis; // Assuming axis is normalized
    Vec3 toPoint = point - center;
    float projection = axisNorm.dot(toPoint);

    // Check if the point is on the caps
    if (std::abs(projection - height) < EPSILON) {
        // Top cap
        return axisNorm;
    } else if (std::abs(projection + height) < EPSILON) {
        // Bottom cap
        return -axisNorm;
    } else {
        // Side surface
        Vec3 radial = toPoint - axisNorm * projection;
        return radial.normalise();
    }
}


bool Cylinder::intersect(const Vec3& rayOrigin, const Vec3& rayDir, Intersection& intersection) const {
    Vec3 oc = rayOrigin - center;
    Vec3 axisNorm = axis; // axis is already normalized in constructor

    // Quadratic coefficients for the side surface
    float A = rayDir.dot(rayDir) - pow(rayDir.dot(axisNorm), 2);
    float B = 2.0f * (rayDir.dot(oc) - rayDir.dot(axisNorm) * oc.dot(axisNorm));
    float C = oc.dot(oc) - pow(oc.dot(axisNorm), 2) - radius * radius;

    float discriminant = B * B - 4.0f * A * C;
    std::vector<std::pair<float, Vec3>> hits;

    // Check for intersection with the cylindrical surface
    if (discriminant >= 0.0f) {
        float sqrtDisc = std::sqrt(discriminant);
        float t1 = (-B - sqrtDisc) / (2.0f * A);
        float t2 = (-B + sqrtDisc) / (2.0f * A);

        if (t1 > t2) std::swap(t1, t2);

        auto isWithinHeight = [&](const Vec3& point) -> bool {
            float projection = axisNorm.dot(point - center);
            return projection >= -height && projection <= height;
        };

        if (t1 > EPSILON) {
            Vec3 p1 = rayOrigin + rayDir * t1;
            if (isWithinHeight(p1)) {
                hits.emplace_back(t1, p1);
            }
        }
        if (t2 > EPSILON) {
            Vec3 p2 = rayOrigin + rayDir * t2;
            if (isWithinHeight(p2)) {
                hits.emplace_back(t2, p2);
            }
        }
    }

    // Check for intersection with the end caps
    float denom = rayDir.dot(axisNorm);

    if (std::abs(denom) > EPSILON) {
        // Bottom cap
        Vec3 capBottomCenter = center - axisNorm * height;
        float t_cap_bottom = (capBottomCenter - rayOrigin).dot(axisNorm) / denom;
        if (t_cap_bottom > EPSILON) {
            Vec3 p_cap_bottom = rayOrigin + rayDir * t_cap_bottom;
            float len = (p_cap_bottom - center).length();
            if (len*len <= radius * radius) {
                hits.emplace_back(t_cap_bottom, p_cap_bottom);
            }
        }

        // Top cap
        Vec3 capTopCenter = center + axisNorm * height;
        float t_cap_top = (capTopCenter - rayOrigin).dot(axisNorm) / denom;
        if (t_cap_top > EPSILON) {
            Vec3 p_cap_top = rayOrigin + rayDir * t_cap_top;
            float len = (p_cap_top - capTopCenter).length();
            if ( len*len <= radius * radius) {
                hits.emplace_back(t_cap_top, p_cap_top);
            }
        }
    }

    // Find the closest valid intersection
    if (!hits.empty()) {
        auto min_hit = std::min_element(hits.begin(), hits.end(),
            [](const std::pair<float, Vec3>& a, const std::pair<float, Vec3>& b) {
                return a.first < b.first;
            });

        intersection.hit = true;
        intersection.t = min_hit->first;
        intersection.hitPoint = min_hit->second;
        intersection.normal = getNormal(intersection.hitPoint);
        intersection.material = material;
        return true;
    }

    return false;
}

BoundingBox Cylinder::getBoundingBox() const {
    Vec3 axisNorm = axis.normalise();
    Vec3 p1 = center - axisNorm * height;
    Vec3 p2 = center + axisNorm * height;

    Vec3 min = Vec3(
        std::min(p1.x, p2.x) - radius,
        std::min(p1.y, p2.y) - radius,
        std::min(p1.z, p2.z) - radius
    );

    Vec3 max = Vec3(
        std::max(p1.x, p2.x) + radius,
        std::max(p1.y, p2.y) + radius,
        std::max(p1.z, p2.z) + radius
    );

    return BoundingBox(min, max);
}

// bool Cylinder::intersect(const Vec3& rayOrigin, const Vec3& rayDir, float& t) const {
//     Vec3 oc = rayOrigin - center;
//     Vec3 axisNorm = axis.normalise();

//     // Decompose ray into parallel and perpendicular components relative to cylinder axis
//     Vec3 rayDirPerpendicular = rayDir - axisNorm * rayDir.dot(axisNorm);
//     Vec3 ocPerpendicular = oc - axisNorm * oc.dot(axisNorm);

//     float a = rayDirPerpendicular.dot(rayDirPerpendicular);
//     float b = 2.0f * ocPerpendicular.dot(rayDirPerpendicular);
//     float c = ocPerpendicular.dot(ocPerpendicular) - radius * radius;

//     float discriminant = b * b - 4.0f * a * c;
//     if (discriminant < 0) return false;

//     float sqrtDiscriminant = std::sqrt(discriminant);
//     float t1 = (-b - sqrtDiscriminant) / (2.0f * a);
//     float t2 = (-b + sqrtDiscriminant) / (2.0f * a);

//     if (t1 > t2) std::swap(t1, t2);  // Ensure t1 is the smaller value

//     // Define bounds of the cylinder along the axis
//     Vec3 bottomCap = center - axisNorm * (height * 0.5f);
//     Vec3 topCap = center + axisNorm * (height * 0.5f);

//     auto isWithinHeight = [&](const Vec3& point) -> bool {
//         float projection = axisNorm.dot(point - center);
//         return projection >= -1.0f * height && projection <= 1.0f * height;
//     };

//     if (t1 > EPSILON) {
//         Vec3 hitPoint = rayOrigin + rayDir * t1;
//         if (isWithinHeight(hitPoint)) {
//             t = t1;
//             return true;
//         }
//     }

//     if (t2 > EPSILON) {
//         Vec3 hitPoint = rayOrigin + rayDir * t2;
//         if (isWithinHeight(hitPoint)) {
//             t = t2;
//             return true;
//         }
//     }

//     return false;
// }


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
    (void)hitPoint; // Explicitly ignore the unused parameter
    Vec3 edge1 = v1 - v0;
    Vec3 edge2 = v2 - v0;
    return edge1.cross(edge2).normalise();
}

bool Triangle::intersect(const Vec3& rayOrigin, const Vec3& rayDir, Intersection& intersection) const {
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

    float t = f * edge2.dot(q);

    if (t > EPSILON) {
        intersection.hit = true;
        intersection.t = t;
        intersection.hitPoint = rayOrigin + rayDir * t;
        intersection.normal = getNormal(intersection.hitPoint);
        intersection.material = material;
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
