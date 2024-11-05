#include "raytracer.h"

// Vec3 method definitions
Vec3 Vec3::operator+(const Vec3& other) const {
    return {x + other.x, y + other.y, z + other.z};
}

Vec3 Vec3::operator-(const Vec3& other) const {
    return {x - other.x, y - other.y, z - other.z};
}

Vec3 Vec3::operator*(float scalar) const {
    return {x * scalar, y * scalar, z * scalar};
}

Vec3 Vec3::cross(const Vec3& other) const {
    return {y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x};
}

Vec3 Vec3::normalize() const {
    float length = std::sqrt(x * x + y * y + z * z);
    return {x / length, y / length, z / length};
}

float Vec3::dot(const Vec3& other) const {
    return x * other.x + y * other.y + z * other.z;
}

// Camera class definition

Camera::Camera(const Vec3& pos, const Vec3& look, const Vec3& upDir, int w, int h, float fieldOfView, float expos, float aspect)
    : position(pos), lookAt(look), upVector(upDir), width(w), height(h), fov(fieldOfView), exposure(expos), aspectRatio(aspect) {}

Vec3 Camera::getRayDirection(int pixelX, int pixelY) const {
    float scale = std::tan(fov * 0.5 * M_PI / 180);
    float imageAspectRatio = static_cast<float>(width) / height;
    Vec3 forward = (lookAt - position).normalize();
    Vec3 right = forward.cross(upVector).normalize();
    Vec3 cameraUp = right.cross(forward);
    float ndcX = (2 * (pixelX + 0.5) / width - 1) * imageAspectRatio * scale;
    float ndcY = (1 - 2 * (pixelY + 0.5) / height) * scale;
    Vec3 rayDir = (forward + right * ndcX + cameraUp * ndcY).normalize();
    return rayDir;
}

void Camera::printCameraSpecs() {
    std::cout << "Camera Info:" << std::endl;
    std::cout << "position (x,y,z): " << position.x <<" "<< position.y << " " << position.z <<std::endl;
    std::cout << "looking at (x,y,z): " << lookAt.x <<" "<< lookAt.y << " " << lookAt.z <<std::endl;
    std::cout << "up vector (x,y,z): " << upVector.x <<" "<< upVector.y << " " << upVector.z <<std::endl;
    std::cout << "width: " << width << std::endl;
    std::cout << "height: " << height << std::endl;
    std::cout << "fov: " << fov << std::endl;
    std::cout << "aspect ratio: " << aspectRatio << std::endl;
    std::cout << "exposure: " << exposure << std::endl;
    std::cout << std::endl;
}

// Sphere class definition
Sphere::Sphere(const Vec3 &centerPos, float rad) : center(centerPos), radius(rad) {}

bool Sphere::intersect(const Vec3& rayOrigin, const Vec3& rayDir) const {
    Vec3 oc = rayOrigin - center;
    float a = rayDir.dot(rayDir);
    float b = 2.0f * oc.dot(rayDir);
    float c = oc.dot(oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;
    return (discriminant > 0);
}

void Sphere::printInfo() const {
    std::cout << "Sphere Info:" << std::endl;
    std::cout << "center (x,y,z): " << center.x <<" "<< center.y << " " << center.z <<std::endl;
    std::cout << "radius: " << radius << std::endl;
}

// Cylinder class definition
Cylinder::Cylinder(const Vec3& centerPos, const Vec3& ax, float rad, float h)
: center(centerPos), axis(ax.normalize()), radius(rad), height(h) {}

bool Cylinder::intersect(const Vec3& rayOrigin, const Vec3& rayDir) const {
    // Simplified intersection method (detailed implementation would be more complex)
    return false;  // Placeholder
}

void Cylinder::printInfo() const {
    std::cout << "Cylinder Info:" << std::endl;
    std::cout << "center (x,y,z): " << center.x <<" "<< center.y << " " << center.z <<std::endl;
    std::cout << "axis (x,y,z): " << axis.x <<" "<< axis.y << " " << axis.z <<std::endl;
    std::cout << "radius: " << radius << std::endl;
    std::cout << "height: " << height << std::endl;
}

// Triangle class definition
Triangle::Triangle(const Vec3& vertex0, const Vec3& vertex1, const Vec3& vertex2)
: v0(vertex0), v1(vertex1), v2(vertex2) {}

bool Triangle::intersect(const Vec3& rayOrigin, const Vec3& rayDir) const {
    // Simplified intersection method (detailed implementation would be more complex)
    return false;  // Placeholder
}

void Triangle::printInfo() const {
    std::cout << "Triangle Info:" << std::endl;
    std::cout << "v0 (x,y,z): " << v0.x <<" "<< v0.y << " " << v0.z <<std::endl;
    std::cout << "v1 (x,y,z): " << v1.x <<" "<< v1.y << " " << v1.z <<std::endl;
    std::cout << "v2 (x,y,z): " << v2.x <<" "<< v2.y << " " << v2.z <<std::endl;
}

Colour calculateColour(const Vec3& rayDirection) {
    // Example colour based on ray direction: converting direction to colour components
    int r = static_cast<int>((rayDirection.x + 1) * 127.5);  // Mapping -1 to 1 range to 0-255
    int g = static_cast<int>((rayDirection.y + 1) * 127.5);
    int b = static_cast<int>((rayDirection.z + 1) * 127.5);
    return Colour(r, g, b);
}

// Scene class definition
Scene::Scene(const json& jsonData) {
    json jsonCamera;
    if (jsonData.contains("camera")) {
        jsonCamera = jsonData["camera"];
    } else {
        // Error in accessing camera in json data
        std::cerr << "Error in Scene init: 'camera' is missing from json data." << std::endl;
        throw std::runtime_error("'camera' data is missing in JSON");  // Throw an exception
    }
    json jsonScene;
    if (jsonData.contains("scene")) {
        jsonScene = jsonData["scene"];
    } else {
        // Error in accessing camera in json data
        std::cerr << "Error in Scene init: 'scene' is missing from json data." << std::endl;
        throw std::runtime_error("'scene' data is missing in JSON");  // Throw an exception
    }
    // Initialise camera position in the scene
    Vec3 cameraPos;
    if (jsonCamera.contains("position") && jsonCamera["position"].is_array() && jsonCamera["position"].size() == 3) {
        cameraPos.x = jsonCamera["position"][0].get<float>();
        cameraPos.y = jsonCamera["position"][1].get<float>();
        cameraPos.z = jsonCamera["position"][2].get<float>();
    } else {
        // Error in accessing camera position
        std::cerr << "Error in Scene init: 'position' is missing or does not contain exactly three elements." << std::endl;
        cameraPos = {0.0f, 0.0f, 0.0f};  // Default position
    }
    // Initialise camera look at vector
    Vec3 cameraLookAt;
    if (jsonCamera.contains("lookAt") && jsonCamera["lookAt"].is_array() && jsonCamera["lookAt"].size() == 3) {
        cameraLookAt.x = jsonCamera["lookAt"][0].get<float>();
        cameraLookAt.y = jsonCamera["lookAt"][1].get<float>();
        cameraLookAt.z = jsonCamera["lookAt"][2].get<float>();
    } else {
        // Error in accessing camera direction
        std::cerr << "Error in Scene init: 'lookAt' is missing or does not contain exactly three elements." << std::endl;
        cameraLookAt = {0.0f, 0.0f, -1.0f};  // Default direction
    }
    // Initialise camera up vector
    Vec3 cameraUp;
    if (jsonCamera.contains("upVector") && jsonCamera["upVector"].is_array() && jsonCamera["upVector"].size() == 3) {
        cameraUp.x = jsonCamera["upVector"][0].get<float>();
        cameraUp.y = jsonCamera["upVector"][1].get<float>();
        cameraUp.z = jsonCamera["upVector"][2].get<float>();
    } else {
        // Error in accessing camera up vector
        std::cerr << "Error in Scene init: 'upVector' is missing or does not contain exactly three elements." << std::endl;
        cameraUp = {0.0f, 1.0f, 0.0f};  // Default camera up vector
    }
    // Initialise width with default value 800 if missing
    int width = jsonCamera.contains("width") && jsonCamera["width"].is_number_integer() ? 
                jsonCamera["width"].get<int>() : 
                (std::cerr << "Error in Scene init: 'width' is missing or not an integer." << std::endl, 800);

    // Initialise height with default value 600 if missing
    int height = jsonCamera.contains("height") && jsonCamera["height"].is_number_integer() ? 
                 jsonCamera["height"].get<int>() : 
                 (std::cerr << "Error in Scene init: 'height' is missing or not an integer." << std::endl, 600);

    // Initialise fov with default value 90.0 if missing
    float fov = jsonCamera.contains("fov") && jsonCamera["fov"].is_number_float() ? 
                jsonCamera["fov"].get<float>() : 
                (std::cerr << "Error in Scene init: 'fov' is missing or not a float." << std::endl, 90.0f);

    // Initialise exposure with default value 0.1 if missing
    float exposure = jsonCamera.contains("exposure") && jsonCamera["exposure"].is_number_float() ? 
                     jsonCamera["exposure"].get<float>() : 
                     (std::cerr << "Error in Scene init: 'exposure' is missing or not a float." << std::endl, 0.1f);
    float aspectRatio = static_cast<float>(width) / height;

    // Construct scene camera
    camera = Camera(cameraPos, cameraLookAt, cameraUp, width, height, fov, exposure, aspectRatio);
    
    if (jsonScene.contains("backgroundcolor") && jsonScene["backgroundcolor"].is_array() && jsonScene["backgroundcolor"].size() == 3) {
        backgroundColour = Colour(
            jsonScene["backgroundcolor"][0].get<float>() * 255,
            jsonScene["backgroundcolor"][1].get<float>() * 255,
            jsonScene["backgroundcolor"][2].get<float>() * 255
        );
    } else {
        // Error in accessing background colour
        std::cerr << "Error in Scene init: 'backgroundcolor' is missing or does not contain exactly three elements." << std::endl;
        backgroundColour = {0, 0, 0};  // Default colour
    }

    if (jsonScene.contains("shapes") && jsonScene["shapes"].is_array()) {
        for (const auto& shapeData : jsonScene["shapes"]) {
            std::string type = shapeData["type"];
            if (type == "sphere") {
                Vec3 center = {
                    shapeData["center"][0].get<float>(),
                    shapeData["center"][1].get<float>(),
                    shapeData["center"][2].get<float>()
                };
                float radius = shapeData["radius"].get<float>();
                shapes.push_back(std::make_shared<Sphere>(center, radius));
            } else if (type == "cylinder") {
                Vec3 center = {
                    shapeData["center"][0].get<float>(),
                    shapeData["center"][1].get<float>(),
                    shapeData["center"][2].get<float>()
                };
                Vec3 axis = {
                    shapeData["axis"][0].get<float>(),
                    shapeData["axis"][1].get<float>(),
                    shapeData["axis"][2].get<float>()
                };
                float radius = shapeData["radius"].get<float>();
                float height = shapeData["height"].get<float>();
                shapes.push_back(std::make_shared<Cylinder>(center, axis, radius, height));
            } else if (type == "triangle") {
                Vec3 v0 = {
                    shapeData["v0"][0].get<float>(),
                    shapeData["v0"][1].get<float>(),
                    shapeData["v0"][2].get<float>()
                };
                Vec3 v1 = {
                    shapeData["v1"][0].get<float>(),
                    shapeData["v1"][1].get<float>(),
                    shapeData["v1"][2].get<float>()
                };
                Vec3 v2 = {
                    shapeData["v2"][0].get<float>(),
                    shapeData["v2"][1].get<float>(),
                    shapeData["v2"][2].get<float>()
                };
                shapes.push_back(std::make_shared<Triangle>(v0, v1, v2));
            }
        }
    } else {
        // Error in accessing background colour
        std::cerr << "Error in Scene init: 'shapes' is missing or does not contain valid shape data." << std::endl;
    }
    std::cout << "Successfully initialised scene!" << std::endl;
}

void Scene::renderScene(std::vector<Colour>& pixels) {
    Vec3 sphereCenter = {0, 1, -3}; // Position a sphere in front of the camera
    float sphereRadius = 1.0f;
    Sphere sphere1 = Sphere(sphereCenter, sphereRadius);

    #pragma omp parallel for
    for (int y = 0; y < camera.height; ++y) {
        for (int x = 0; x < camera.width; ++x) {
            Vec3 rayDir = camera.getRayDirection(x, y);
            Vec3 rayOrigin = camera.position;

            // Check if the ray intersects the sphere
            if (sphere1.intersect(rayOrigin, rayDir)) {
                pixels[y * camera.width + x] = Colour(255, 0, 0); // Colour pixels red where intersection occurs
            } else {
                pixels[y * camera.width + x] = calculateColour(rayDir); // Background gradient
            }
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
