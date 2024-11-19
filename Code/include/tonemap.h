#ifndef TONEMAP_H
#define TONEMAP_H

#include "common.h"

Colour linearToneMap(const Colour& hdrColour, float exposure);
Vec3 ACESFittedToneMap(const Vec3& color, float exposure);
Colour reinhardToneMap(const Colour& hdrColour, float exposure);
Colour gammaCorrect(const Colour& linearColour, float gamma = 2.2f);
std::vector<int> computeHistogram(const std::vector<Colour>& pixels, int bins = 256);
std::vector<float> computeCDF(const std::vector<int>& histogram);
void applyHistogramEqualisation(std::vector<Colour>& pixels, float exposure);

#endif