#include "scene.h"


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
    std::string cameraType = jsonCamera.value("type", "pinhole");
    Vec3 cameraPos = getVec3FromJson(jsonCamera, "position", {0.0f, 0.0f, 0.0f});
    Vec3 cameraLookAt = getVec3FromJson(jsonCamera, "lookAt", {0.0f, 0.0f, -1.0f});
    Vec3 cameraUp = getVec3FromJson(jsonCamera, "upVector", {0.0f, 1.0f, 0.0f});
    int width = jsonCamera.value("width", 800);
    int height = jsonCamera.value("height", 600);
    float fov = jsonCamera.value("fov", 90.0f);
    float exposure = jsonCamera.value("exposure", 0.1f);
    float aspectRatio = static_cast<float>(width) / height;

    
    // Check for camera type
    if (renderMode == RenderMode::BINARY) {
        std::cout << "Using pinhole camera." << std::endl;
        camera = std::make_shared<PinholeCamera>(cameraPos, cameraLookAt, cameraUp, width, height, fov, exposure, aspectRatio); 
    } else if (renderMode == RenderMode::PHONG) {
        std::cout << "Using pinhole camera." << std::endl;
        camera = std::make_shared<PinholeCamera>(cameraPos, cameraLookAt, cameraUp, width, height, fov, exposure, aspectRatio); 
    } else if (renderMode == RenderMode::PATH) {
        std::cout << "Using aperature camera." << std::endl;
        float apertureSize = jsonCamera.value("apertureSize", 0.01f);
        float focalDistance = jsonCamera.value("focalDistance", 2.0f);
        camera = std::make_shared<ApertureCamera>(cameraPos, cameraLookAt, cameraUp, width, height, fov, exposure, aspectRatio, apertureSize, focalDistance);
    }

    // Parse background color
    json jsonScene = jsonData.at("scene");
    backgroundColour = getColourFromJson(jsonScene, "backgroundcolor",  Colour(64, 64, 64));

    // Parse lights
    if (jsonScene.contains("lightsources") && jsonScene["lightsources"].is_array()) {
        for (const auto& lightData : jsonScene["lightsources"]) {
            // Extract type
            std::string type = lightData.value("type", "");

            // Parse position and intensity
            Vec3 position = getVec3FromJson(lightData, "position", {0, 0, 0});
            Colour intensity = getColourFromJson(lightData, "intensity", Colour(255, 255, 255));

            if (renderMode == RenderMode::BINARY) {
                // Create a PointLight
                std::cout << "Using pointlights." << std::endl;
                lights.emplace_back(std::make_shared<PointLight>(position, intensity));  
            } else if (renderMode == RenderMode::PHONG){
                std::cout << "Using pointlights." << std::endl;
                lights.emplace_back(std::make_shared<PointLight>(position, intensity));
            } else if (renderMode == RenderMode::PATH) {
                std::cout << "Using area lights." << std::endl;
                Vec3 u = getVec3FromJson(lightData, "u", {1, 0, 0});  // Default to x-axis
                Vec3 v = getVec3FromJson(lightData, "v", {0, 1, 0});  // Default to y-axis
                float width = lightData.value("width", 1.0f);
                float height = lightData.value("height", 1.0f);
                lights.emplace_back(std::make_shared<AreaLight>(position, intensity, u, v, width, height));
            } else {
                std::cerr << "Unknown light type: " << type << std::endl;
            }
        }
    }

    // Parse shapes
    if (jsonScene.contains("shapes") && jsonScene["shapes"].is_array()) {
        for (const auto& shapeData : jsonScene["shapes"]) {
            std::string type = shapeData["type"];
            // Check if "material" key exists, otherwise use a default material
            std::shared_ptr<Material> material;
            if (shapeData.contains("material")) {
                material = std::make_shared<Material>(parseMaterial(shapeData.at("material")));
            } else {
                std::cout << "Material not found for shape, using default material.\n";
                material = std::make_shared<Material>(); // Or set to some default material values
            }

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

                auto calculateUV = [&](const Vec3& v) -> std::pair<float, float> {
                    // Find the minimum values across all triangles
                    float minX = std::min({v0.x, v1.x, v2.x});
                    float minZ = std::min({v0.z, v1.z, v2.z});

                    // Shift all the coordinates to positive space
                    float shiftedX = v.x - minX;
                    float shiftedZ = v.z - minZ;

                    // Normalize to [0, 1)
                    float rangeX = std::max({v0.x - minX, v1.x - minX, v2.x - minX});
                    float rangeZ = std::max({v0.z - minZ, v1.z - minZ, v2.z - minZ});

                    // Ensure range is not zero to avoid division by zero
                    rangeX = rangeX == 0 ? 1 : rangeX;
                    rangeZ = rangeZ == 0 ? 1 : rangeZ;

                    return {shiftedX / rangeX, shiftedZ / rangeZ};
                };
                std::pair<float, float> uv0 = calculateUV(v0);
                std::pair<float, float> uv1 = calculateUV(v1);
                std::pair<float, float> uv2 = calculateUV(v2);

                // std::cout << uv0.first << " " << uv0.second << " " << uv1.first << " " << uv1.second << " " << uv2.first << " " << uv2.second << std::endl;
                // Add the triangle to the list of shapes
                shapes.push_back(std::make_shared<Triangle>(v0, v1, v2, uv0, uv1, uv2, material));
            }
        }
    }
    bvh.build(shapes);

    std::cout << "Successfully initialised scene with render mode: " << rendermodeStr << "!" << std::endl;
}

bool Scene::intersect(const Ray& ray, Intersection& nearestIntersection) const {
    return bvh.traverse(ray, nearestIntersection);
}


void Scene::renderScene(std::vector<Colour>& pixels) {

    std::shared_ptr<RayTracer> rayTracer = std::make_shared<BinaryTracer>();

    switch (renderMode) {
        case RenderMode::BINARY:
            rayTracer = std::make_shared<BinaryTracer>();
            break;
        case RenderMode::PHONG:
            rayTracer = std::make_shared<PhongTracer>();
            break;
        case RenderMode::PATH:
            rayTracer = std::make_shared<PathTracer>();
            break;
        default:
            std::cerr << "Unknown render mode. Defaulting to BinaryTracer" << std::endl;
            rayTracer = std::make_shared<BinaryTracer>();
            break;
    }

    // Render using the selected ray tracer
    rayTracer->renderScene(*this, pixels);
}

void Scene::printSceneInfo() {
    camera->printCameraSpecs();
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
    return {camera->width, camera->height};
}