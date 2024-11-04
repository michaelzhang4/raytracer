#include "raytracer.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <vector>
#include <omp.h>


Colour calculateColour(const Vec3& rayDirection) {
    // Example colour based on ray direction: converting direction to colour components
    int r = static_cast<int>((rayDirection.x + 1) * 127.5);  // Mapping -1 to 1 range to 0-255
    int g = static_cast<int>((rayDirection.y + 1) * 127.5);
    int b = static_cast<int>((rayDirection.z + 1) * 127.5);
    return Colour(r, g, b);
}

// In `renderScene`, check for intersections
void renderScene(std::vector<Colour>& pixels, int width, int height, const Camera& camera) {
    Vec3 sphereCenter = {0, 1, -3}; // Position a sphere in front of the camera
    float sphereRadius = 1.0f;
    Sphere sphere1 = Sphere(sphereCenter, sphereRadius);

    #pragma omp parallel for
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Vec3 rayDir = camera.getRayDirection(x, y, width, height);
            Vec3 rayOrigin = camera.position;

            // Check if the ray intersects the sphere
            if (sphere1.intersectSphere(rayOrigin, rayDir)) {
                pixels[y * width + x] = Colour(255, 0, 0); // Colour pixels red where intersection occurs
            } else {
                pixels[y * width + x] = calculateColour(rayDir); // Background gradient
            }
        }
    }
}

// Creates ppm images from buffer of pixels containing RBG colours
void writePPM(const std::string &filename, const std::vector<Colour> &pixels, int width, int height) {
    std::ofstream outFile(filename, std::ios::out | std::ios::binary);
    if (!outFile) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }
    // Define P3 format, width, height and colour range 0-255
    outFile << "P3\n" << width << " " << height << "\n255\n";
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            const Colour &colour = pixels[i * width + j];
            outFile << colour.r << " " << colour.g << " " << colour.b << " ";
        }
        outFile << "\n";
    }
    outFile.close();
}
