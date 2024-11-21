#include "camera.h"

Camera::Camera(const Vec3& pos, const Vec3& look, const Vec3& upVec, int w, int h, float fieldOfView, float expos, float aspect)
    : position(pos), lookAt(look), upVector(upVec), width(w), height(h), fov(fieldOfView), exposure(expos), aspectRatio(aspect) {}

Vec3 PinholeCamera::getRayDirection(int pixelX, int pixelY) const {
    float scale = std::tan(fov * 0.5 * M_PI / 180);
    float imageAspectRatio = static_cast<float>(width) / height;
    Vec3 forward = (lookAt - position).normalise();
    Vec3 right = upVector.cross(forward).normalise();
    Vec3 cameraUp = forward.cross(right);
    float ndcX = (2 * (pixelX + 0.5) / width - 1) * imageAspectRatio * scale;
    float ndcY = (1 - 2 * (pixelY + 0.5) / height) * scale;
    return (forward + right * ndcX + cameraUp * ndcY).normalise();
}

Vec3 ApertureCamera::getRayDirection(int pixelX, int pixelY) const {
    // Generate ray direction as if it's a pinhole camera
    float scale = std::tan(fov * 0.5 * M_PI / 180);
    float imageAspectRatio = static_cast<float>(width) / height;
    Vec3 forward = (lookAt - position).normalise();
    Vec3 right = upVector.cross(forward).normalise();
    Vec3 cameraUp = forward.cross(right);
    float ndcX = (2 * (pixelX + 0.5) / width - 1) * imageAspectRatio * scale;
    float ndcY = (1 - 2 * (pixelY + 0.5) / height) * scale;
    Vec3 rayDir = (forward + right * ndcX + cameraUp * ndcY).normalise();

    // Apply depth of field effect
    Vec3 focalPoint = position + rayDir * focalDistance;
    float lensRadius = apertureSize / 2.0f;

    // Random point on the lens
    float r = lensRadius * std::sqrt(static_cast<float>(rand()) / RAND_MAX);
    float theta = 2.0f * M_PI * static_cast<float>(rand()) / RAND_MAX;

    float dx = r * std::cos(theta);
    float dy = r * std::sin(theta);

    Vec3 lensOffset(dx, dy, 0.0f); // Assuming lens lies on the xy-plane
    Vec3 newOrigin = position + lensOffset;

    return (focalPoint - newOrigin).normalise();
}


void Camera::printCameraSpecs() {
    std::cout << "Camera Info:" << std::endl;
    std::cout << "position (x,y,z): " << position.x << " " << position.y << " " << position.z << std::endl;
    std::cout << "looking at (x,y,z): " << lookAt.x << " " << lookAt.y << " " << lookAt.z << std::endl;
    std::cout << "up vector (x,y,z): " << upVector.x << " " << upVector.y << " " << upVector.z << std::endl;
    std::cout << "width: " << width << std::endl;
    std::cout << "height: " << height << std::endl;
    std::cout << "fov: " << fov << std::endl;
    std::cout << "aspect ratio: " << aspectRatio << std::endl;
    std::cout << "exposure: " << exposure << std::endl;
    std::cout << std::endl;
}
