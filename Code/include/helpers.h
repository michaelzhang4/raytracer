#ifndef HELPERS_H
#define HELPERS_H

#include "common.h"
#include "material.h"

// Function declarations
void writePPM(const std::string &filename, const std::vector<Colour> &pixels, int width, int height);
std::pair<std::vector<Colour>, std::pair<int, int>> readPPM(const std::string &filename);
Vec3 getVec3FromJson(const nlohmann::json& jsonObj, const std::string& key, const Vec3& defaultVal);
RenderMode stringToRenderMode(const std::string& mode);
Colour getColourFromJson(const json& jsonObj, const std::string& key, const Colour& defaultVal);
Material parseMaterial(const json& materialData);

#endif
