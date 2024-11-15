#include "common.h"

class Texture {
public:
    int width, height;
    std::vector<Colour> pixels;  // Store pixel data as Colour objects

    Texture(const std::string& filename);
    Colour getPixel(int u, int v) const;  // Get a pixel at (u, v)
    Colour sample(float u, float v) const; // Sample texture using UV coordinates
};
