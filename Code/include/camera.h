#ifndef CAMERA_H
#define CAMERA_H

#include "common.h"

class Camera {
public:
    Vec3 position;
    Vec3 lookAt;
    Vec3 upVector;
    int width;
    int height;
    float fov;
    float exposure;
    float aspectRatio;

    // Default constructor
    Camera() 
        : position({0.0f, 0.0f, 0.0f}), lookAt({0.0f, 0.0f, -1.0f}), upVector({0.0f, 1.0f, 0.0f}),
          width(800), height(600), fov(90.0f), exposure(1.0f), aspectRatio(4.0f / 3.0f) {}

    // Parameterised constructor
    Camera(const Vec3& pos, const Vec3& look, const Vec3& upVec, int w, int h, float fieldOfView, float expos, float aspect);

    // Generate ray direction based on pixel position
    virtual Vec3 getRayDirection(int pixelX, int pixelY) const = 0;

    // Print camera specifications
    void printCameraSpecs();

    virtual ~Camera() = default;
};

// Pinhole Camera class
class PinholeCamera : public Camera {
public:
    // Constructor
    PinholeCamera(const Vec3& pos, const Vec3& look, const Vec3& upVec, int w, int h, float fieldOfView, float expos, float aspect)
        : Camera(pos, look, upVec, w, h, fieldOfView, expos, aspect) {}

    // Override getRayDirection
    Vec3 getRayDirection(int pixelX, int pixelY) const override;

};

// Aperture Camera class
class ApertureCamera : public Camera {
public:
    float apertureSize; // Size of the aperture
    float focalDistance; // Focal plane distance

    // Constructor
    ApertureCamera(const Vec3& pos, const Vec3& look, const Vec3& upVec, int w, int h, float fieldOfView, float expos, float aspect, float aperture, float focalDist)
        : Camera(pos, look, upVec, w, h, fieldOfView, expos, aspect), apertureSize(aperture), focalDistance(focalDist) {}

    // Override getRayDirection
    Vec3 getRayDirection(int pixelX, int pixelY) const override;
};

#endif // CAMERA_H
