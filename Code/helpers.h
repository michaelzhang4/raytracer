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
Colour linearToneMap(const Colour& hdrColour, float exposure);
Vec3 ACESFittedToneMap(const Vec3& color, float exposure);
Colour reinhardToneMap(const Colour& hdrColour, float exposure);
Colour gammaCorrect(const Colour& linearColour, float gamma = 2.2f);
std::vector<int> computeHistogram(const std::vector<Colour>& pixels, int bins = 256);
std::vector<float> computeCDF(const std::vector<int>& histogram);
void applyHistogramEqualisation(std::vector<Colour>& pixels, float exposure);

#endif
