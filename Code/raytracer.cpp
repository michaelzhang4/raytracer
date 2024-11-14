#include "raytracer.h"
#include "scene.h"


void BinaryTracer::renderScene(const Scene& scene, std::vector<Colour>& pixels) const {
    const Camera& camera = scene.getCamera();
    const auto& shapes = scene.getShapes();
    const Colour& backgroundColour = scene.getBackgroundColour();

    // Resize the pixel buffer based on camera dimensions
    pixels.resize(camera.width * camera.height);
    
    // Parallel rendering loop
    #pragma omp parallel for
    for (int y = 0; y < camera.height; ++y) {
        for (int x = 0; x < camera.width; ++x) {
            Vec3 rayDir = camera.getRayDirection(x, y).normalise();
            Vec3 rayOrigin = camera.position;

            Colour pixelColour = backgroundColour;
            float nearestT = std::numeric_limits<float>::max(); // Nearest intersection distance
            std::shared_ptr<Shape> hitObject = nullptr;

            // Check intersection with all shapes
            for (const auto& shape : shapes) {
                float t = 0.0f;
                if (shape->intersect(rayOrigin, rayDir, t)) {
                    if (t < nearestT) {
                        nearestT = t;
                        hitObject = shape;
                    }
                }
            }

            // Binary mode: set red if any object is hit
            if (hitObject) {
                pixelColour = Colour(255, 0, 0);  // Red for hit objects
            }

            pixels[y * camera.width + x] = pixelColour;
        }
    }
}


void PhongTracer::renderScene(const Scene& scene, std::vector<Colour>& pixels) const {
    const Camera& camera = scene.getCamera();
    const auto& shapes = scene.getShapes();
    const Colour& backgroundColour = scene.getBackgroundColour();
    const auto& lights = scene.getLights(); // Assuming you have lights in Scene

    pixels.resize(camera.width * camera.height);

    #pragma omp parallel for
    for (int y = 0; y < camera.height; ++y) {
        for (int x = 0; x < camera.width; ++x) {
            Vec3 rayDir = camera.getRayDirection(x, y).normalise();
            Vec3 rayOrigin = camera.position;

            Colour pixelColour = backgroundColour;
            float nearestT = std::numeric_limits<float>::max();
            std::shared_ptr<Shape> hitObject = nullptr;

            // Find nearest intersection
            for (const auto& shape : shapes) {
                float t = 0.0f;
                if (shape->intersect(rayOrigin, rayDir, t)) {
                    if (t < nearestT) {
                        nearestT = t;
                        hitObject = shape;
                    }
                }
            }

            if (hitObject) {
                Vec3 hitPoint = rayOrigin + rayDir * nearestT;
                Vec3 normal = hitObject->getNormal(hitPoint).normalise();
                Colour finalColour = {0, 0, 0};

                for (const auto& light : lights) {
                    Vec3 lightDir = (light.position - hitPoint).normalise();

                    // Ambient lighting
                    Colour ambient = hitObject->material.diffuseColor * 0.1f;

                    // Diffuse lighting
                    float diffuseIntensity = std::max(0.0f, normal.dot(lightDir));
                    Colour diffuse = hitObject->material.diffuseColor * diffuseIntensity;

                    // Specular lighting
                    Vec3 reflectDir = (normal * (2.0f * normal.dot(lightDir)) - lightDir).normalise();
                    Vec3 viewDir = (camera.position - hitPoint).normalise();
                    float specularIntensity = std::pow(std::max(0.0f, reflectDir.dot(viewDir)), hitObject->material.specularExponent);
                    Colour specular = hitObject->material.specularColor * specularIntensity * hitObject->material.ks;

                    // Combine components
                    finalColour = finalColour + ambient + diffuse * hitObject->material.kd + specular;
                }

                // Clamp colour to [0, 255]
                finalColour.clamp();
                pixelColour = finalColour;
            }

            pixels[y * camera.width + x] = pixelColour;
        }
    }
}