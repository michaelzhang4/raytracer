
#ifndef PARSER_H
#define PARSER_H

#include "common.h"
#include "material.h"

int parse(const std::string& filepath, json &jsonData);
Vec3 getVec3FromJson(const nlohmann::json& jsonObj, const std::string& key, const Vec3& defaultVal);
Colour getColourFromJson(const json& jsonObj, const std::string& key, const Colour& defaultVal);
Material parseMaterial(const json& materialData);
void displayJsonData(const json &jsonData);

#endif