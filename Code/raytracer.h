#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <string>
#include <vector>
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// RGB Colours
struct Colour {
    int r, g, b;
    Colour(int red = 0, int green = 0, int blue = 0) : r(red), g(green), b(blue) {
    }
};

// 3D vector coordinates
struct Vec3 {
    float x, y, z;

    Vec3 operator+(const Vec3& other) const {
        return {x + other.x, y + other.y, z + other.z};
    }
    Vec3 operator-(const Vec3& other) const {
        return {x - other.x, y - other.y, z - other.z};
    }
    Vec3 operator*(float scalar) const {
        return {x * scalar, y * scalar, z * scalar};
    }
    Vec3 cross(const Vec3& other) const {
        return {y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x};
    }
    Vec3 normalize() const {
        float length = std::sqrt(x * x + y * y + z * z);
        return {x / length, y / length, z / length};
    }
    float dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
};


// Camera class with basic transformations
class Camera {
public:
    Vec3 position;  // Camera position in world space
    Vec3 lookAt;    // Target point the camera is looking at
    Vec3 up;        // Up vector for the camera orientation
    float fov;      // Field of view in degrees
    float aspectRatio;

    Camera(const Vec3& pos, const Vec3& look, const Vec3& upDir, float fieldOfView, float aspect)
        : position(pos), lookAt(look), up(upDir), fov(fieldOfView), aspectRatio(aspect) {}

    // Generate ray direction for a given pixel coordinate
    Vec3 getRayDirection(int pixelX, int pixelY, int imageWidth, int imageHeight) const {
        // FOV scale based on how wide the view is (wider means more spread out)
        float scale = std::tan(fov * 0.5 * M_PI / 180);
        float imageAspectRatio = static_cast<float>(imageWidth) / imageHeight;

        // Camera basis vectors
        Vec3 forward = (lookAt - position).normalize();       // Forward direction
        Vec3 right = forward.cross(up).normalize();           // Right direction
        Vec3 cameraUp = right.cross(forward);                 // Orthogonal up direction

        // Pixel values between [-1,1]
        float ndcX = (2 * (pixelX + 0.5) / imageWidth - 1) * imageAspectRatio * scale;
        float ndcY = (1 - 2 * (pixelY + 0.5) / imageHeight) * scale;

        // Transform NDC coordinates to world space
        Vec3 rayDir = (forward + right * ndcX + cameraUp * ndcY).normalize();
        return rayDir;
    }
};

class Sphere {
public:
    Vec3 center;
    float radius;
    Sphere(Vec3 cPos, float r) : center(cPos), radius(r) {}

    bool intersectSphere(const Vec3& rayOrigin, const Vec3& rayDir) {
        Vec3 oc = rayOrigin - center;
        float a = rayDir.dot(rayDir);
        float b = 2.0f * oc.dot(rayDir);
        float c = oc.dot(oc) - radius * radius;
        float discriminant = b * b - 4 * a * c;
    return (discriminant > 0);
}
};

// Function declarations
Colour calculateColour(const Vec3& rayDirection);
void renderScene(std::vector<Colour>& pixels, int width, int height, const Camera& camera);
void writePPM(const std::string &filename, const std::vector<Colour> &pixels, int width, int height);

#endif // RAYTRACER_H
