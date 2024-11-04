#include "raytracer.h"
#include <vector>
#include <iostream>

int main() {
    // Image dimensions (arbitrary can change later)
    const int width = 800;
    const int height = 600;

    // Initialise camera
    Vec3 cameraPos = {0.0f, 0.0f, 0.0f};
    Vec3 cameraLookAt = {0.0f, 0.0f, -1.0f};
    Vec3 cameraUp = {0.0f, 1.0f, 0.0f};
    float fov = 90.0f; // Field of view
    float aspectRatio = static_cast<float>(width) / height;
    Camera camera(cameraPos, cameraLookAt, cameraUp, fov, aspectRatio);

    // Prepare the pixel buffer
    std::vector<Colour> pixels(width * height);

    // Render the scene
    renderScene(pixels, width, height, camera);

    // Save the result to a PPM file
    std::string filename = "output.ppm";
    writePPM(filename, pixels, width, height);

    std::cout << "Rendered image saved to " << filename << std::endl;
    return 0;
}
