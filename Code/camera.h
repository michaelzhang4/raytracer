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
    Vec3 getRayDirection(int pixelX, int pixelY) const;

    // Calculate pixel colour based on render mode
    Colour calculatePixelColour(const Vec3& rayDirection, const std::string& renderMode) const;

    // Print camera specifications
    void printCameraSpecs();

    virtual ~Camera() = default;
};

#endif // CAMERA_H
