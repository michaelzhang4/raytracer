#include "helpers.h"


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

// Helper function to convert string to RenderMode
RenderMode stringToRenderMode(const std::string& mode) {
    if (mode == "binary") {
        return RenderMode::BINARY;
    } else if (mode == "phong") {
        return RenderMode::PHONG;
    } else {
        throw std::runtime_error("Unsupported render mode: " + mode);
    }
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

    return Material(kd, ks, specularExponent, diffuse, specular, isReflective, reflectivity, isRefractive, refractiveIndex);
}

