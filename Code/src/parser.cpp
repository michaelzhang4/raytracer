#include <iostream>
#include <fstream>

#include "parser.h"

int parse(const std::string& filepath, json &jsonData) {
    std::ifstream file(filepath);   // Open file as read
    // Check for file opening error
    if (!file.is_open()) { 
        std::cerr << "Error: Could not open file " << filepath << std::endl;
        return -1;
    }
    // Catch any parsing errors
    try {
        file >> jsonData;  // Parse into json object
        std::cout << "Parsed JSON data successfully." << std::endl;
    } catch (json::parse_error& e) {
        std::cerr << "Error: JSON parsing failed - " << e.what() << std::endl;
        return -1;
    }
    file.close();
    return 0;
}

Vec3 getVec3FromJson(const json& jsonObj, const std::string& key, const Vec3& defaultVal)
 {
    if (jsonObj.contains(key) && jsonObj[key].is_array() && jsonObj[key].size() == 3) {
        return Vec3{
            jsonObj[key][0].get<float>(),
            jsonObj[key][1].get<float>(),
            jsonObj[key][2].get<float>()
        };
    }
    return defaultVal;
}

Colour getColourFromJson(const json& jsonObj, const std::string& key, const Colour& defaultVal) {
    if (jsonObj.contains(key) && jsonObj[key].is_array() && jsonObj[key].size() == 3) {
        return Colour{
            static_cast<int>(jsonObj[key][0].get<float>() * 255),
            static_cast<int>(jsonObj[key][1].get<float>() * 255),
            static_cast<int>(jsonObj[key][2].get<float>() * 255)
        };
    }
    return defaultVal;
}

Material parseMaterial(const json& materialData) {
    float kd = materialData.value("kd", 0.8f);
    float ks = materialData.value("ks", 0.2f);
    int specularExponent = materialData.value("specularexponent", 10);
    Colour diffuse = getColourFromJson(materialData, "diffusecolor", Colour(255, 255, 255));
    Colour specular = getColourFromJson(materialData, "specularcolor", Colour(255, 255, 255));
    bool isReflective = materialData.value("isreflective", false);
    float reflectivity = materialData.value("reflectivity", 1.0f);
    bool isRefractive = materialData.value("isrefractive", false);
    float refractiveIndex = materialData.value("refractiveindex", 1.0f);
    std::string texturePath = materialData.value("texture", "");

    return Material(kd, ks, specularExponent, diffuse, specular, isReflective, reflectivity, isRefractive, refractiveIndex, texturePath);
}

void displayJsonData(const json &jsonData) {
    // Accessing top-level elements
    std::string rendermode = jsonData["rendermode"];
    std::cout << "Render mode: " << rendermode << std::endl;

    // Accessing nested elements in "camera"
    auto camera = jsonData["camera"];
    int width = camera["width"];
    int height = camera["height"];
    float fov = camera["fov"];
    std::cout << "Camera width: " << width << ", height: " << height << ", fov: " << fov << std::endl;

    // Accessing arrays
    std::vector<float> position = camera["position"];
    std::cout << "Camera position: [" << position[0] << ", " << position[1] << ", " << position[2] << "]" << std::endl;

    // Accessing shapes array
    for (const auto& shape : jsonData["scene"]["shapes"]) {
        std::string type = shape["type"];
        std::cout << "Shape type: " << type << std::endl;
        if (type == "sphere") {
            std::vector<float> center = shape["center"];
            float radius = shape["radius"];
            std::cout << "  Sphere center: [" << center[0] << ", " << center[1] << ", " << center[2] << "], radius: " << radius << std::endl;
        }
        // Additional handling for other shape types...
    }
}
