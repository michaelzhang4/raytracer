#ifndef HELPERS_H
#define HELPERS_H

#include "common.h"

// Function declarations
void writePPM(const std::string &filename, const std::vector<Colour> &pixels, int width, int height);
std::pair<std::vector<Colour>, std::pair<int, int>> readPPM(const std::string &filename);
RenderMode stringToRenderMode(const std::string& mode);
float randomFloat();

#endif
