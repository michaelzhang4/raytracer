#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "common.h"
#include "material.h"

// Function declarations
void writePPM(const std::string &filename, const std::vector<Colour> &pixels, int width, int height);
Vec3 getVec3FromJson(const nlohmann::json& jsonObj, const std::string& key, const Vec3& defaultVal);
RenderMode stringToRenderMode(const std::string& mode);
Colour getColourFromJson(const json& jsonObj, const std::string& key, const Colour& defaultVal);
Material parseMaterial(const json& materialData);

#endif
