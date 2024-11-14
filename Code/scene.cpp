#include "scene.h"
#include "helpers.h"

Scene::Scene(const json& jsonData) {
    std::string rendermodeStr = jsonData.contains("rendermode") ? jsonData["rendermode"].get<std::string>() : "binary";
    renderMode = stringToRenderMode(rendermodeStr);

    if (jsonData.contains("nbounces")) {
        nbounces = jsonData["nbounces"].get<int>();
    } else {
        nbounces = 1;  // Default value
    }

    // Parse camera
    json jsonCamera = jsonData.at("camera");
    Vec3 cameraPos = getVec3FromJson(jsonCamera, "position", {0.0f, 0.0f, 0.0f});
    Vec3 cameraLookAt = getVec3FromJson(jsonCamera, "lookAt", {0.0f, 0.0f, -1.0f});
    Vec3 cameraUp = getVec3FromJson(jsonCamera, "upVector", {0.0f, 1.0f, 0.0f});
    int width = jsonCamera.value("width", 800);
    int height = jsonCamera.value("height", 600);
    float fov = jsonCamera.value("fov", 90.0f);
    float exposure = jsonCamera.value("exposure", 0.1f);
    float aspectRatio = static_cast<float>(width) / height;

    camera = Camera(cameraPos, cameraLookAt, cameraUp, width, height, fov, exposure, aspectRatio);

    // Parse background color
    json jsonScene = jsonData.at("scene");
    backgroundColour = getColourFromJson(jsonScene, "backgroundcolor",  Colour(64, 64, 64));

    // Parse lights
    if (jsonScene.contains("lightsources") && jsonScene["lightsources"].is_array()) {
        for (const auto& lightData : jsonScene["lightsources"]) {
            Vec3 position = getVec3FromJson(lightData, "position", {0, 0, 0});
            Colour intensity = getColourFromJson(lightData, "intensity", Colour(255, 255, 255));
            lights.emplace_back(position, intensity);
        }
    }

    // Parse shapes
    if (jsonScene.contains("shapes") && jsonScene["shapes"].is_array()) {
        for (const auto& shapeData : jsonScene["shapes"]) {
            std::string type = shapeData["type"];
            auto material = parseMaterial(shapeData.at("material"));

            if (type == "sphere") {
                Vec3 center = getVec3FromJson(shapeData, "center", {0, 0, 0});
                float radius = shapeData["radius"].get<float>();
                shapes.push_back(std::make_shared<Sphere>(center, radius, material));
            } else if (type == "cylinder") {
                Vec3 center = getVec3FromJson(shapeData, "center", {0, 0, 0});
                Vec3 axis = getVec3FromJson(shapeData, "axis", {0, 1, 0});
                float radius = shapeData["radius"].get<float>();
                float height = shapeData["height"].get<float>();
                shapes.push_back(std::make_shared<Cylinder>(center, axis, radius, height, material));
            } else if (type == "triangle") {
                Vec3 v0 = getVec3FromJson(shapeData, "v0", {0, 0, 0});
                Vec3 v1 = getVec3FromJson(shapeData, "v1", {1, 0, 0});
                Vec3 v2 = getVec3FromJson(shapeData, "v2", {0, 1, 0});
                shapes.push_back(std::make_shared<Triangle>(v0, v1, v2, material));
            }
        }
    }

    std::cout << "Successfully initialised scene with render mode: " << rendermodeStr << "!" << std::endl;
}

void Scene::renderScene(std::vector<Colour>& pixels) {
    pixels.resize(camera.width * camera.height);
    Vec3 lightDir = {1.0f, 1.0f, -1.0f};
    lightDir = lightDir.normalise();
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

            // Calculate colour based on the render mode
            if (hitObject) {
                if (renderMode == RenderMode::BINARY) {
                    pixelColour = Colour(255, 0, 0);  // Red for hit objects
                } else if (renderMode == RenderMode::PHONG) {
                    std::cout << "HIT" << std::endl;
                    Vec3 hitPoint = rayOrigin + rayDir * nearestT;
                    Vec3 normal = (hitPoint - dynamic_cast<Sphere*>(hitObject.get())->center).normalise();  // Example for sphere
                    float intensity = std::max(0.0f, normal.dot(lightDir));
                    int shade = static_cast<int>(intensity * 255);
                    pixelColour = Colour(shade, shade, shade);  // Grayscale shading
                }
            }

            pixels[y * camera.width + x] = pixelColour;
        }
    }
}

void Scene::printSceneInfo() {
    camera.printCameraSpecs();
    std::cout << "Background colour (r,g,b): " << backgroundColour.r <<" "<< backgroundColour.g << " " << backgroundColour.b <<std::endl;
    for (const auto& shape: shapes) {
        std::cout << std::endl;
        shape->printInfo();
    }
    std::cout << std::endl << "Render mode: ";
    if (renderMode == RenderMode::BINARY) {
        std::cout << "Binary" << std::endl;
    } else if (renderMode == RenderMode::PHONG) {
        std::cout << "Blinn-Phong" << std::endl;
    }
}

std::pair<int, int> Scene::sceneWidthHeight() {
    return {camera.width, camera.height};
}