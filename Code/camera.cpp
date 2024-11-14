#include "camera.h"

Camera::Camera(const Vec3& pos, const Vec3& look, const Vec3& upVec, int w, int h, float fieldOfView, float expos, float aspect)
    : position(pos), lookAt(look), upVector(upVec), width(w), height(h), fov(fieldOfView), exposure(expos), aspectRatio(aspect) {}

Vec3 Camera::getRayDirection(int pixelX, int pixelY) const {
    float scale = std::tan(fov * 0.5 * M_PI / 180);
    float imageAspectRatio = static_cast<float>(width) / height;
    Vec3 forward = (lookAt - position).normalise();
    Vec3 right = upVector.cross(forward).normalise();
    Vec3 cameraUp = forward.cross(right);
    float ndcX = (2 * (pixelX + 0.5) / width - 1) * imageAspectRatio * scale;
    float ndcY = (1 - 2 * (pixelY + 0.5) / height) * scale;
    return (forward + right * ndcX + cameraUp * ndcY).normalise();
}

Colour Camera::calculatePixelColour(const Vec3& rayDirection, const std::string& renderMode) const {
    if (renderMode == "binary") {
        return rayDirection.z > 0 ? Colour(255, 255, 255) : Colour(0, 0, 0);  // Simple binary colour
    } else if (renderMode == "phong") {
        Vec3 lightDir = {1.0f, 1.0f, -1.0f};
        lightDir = lightDir.normalise();
        float intensity = std::max(0.0f, rayDirection.dot(lightDir));
        int grey = static_cast<int>(intensity * 255);
        return Colour(grey, grey, grey);  // Phong-like shading
    } else {
        throw std::runtime_error("Unknown render mode");
    }
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