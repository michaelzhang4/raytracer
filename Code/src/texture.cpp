#include "texture.h"

Texture::Texture(const std::string& filename) {
    if (filename.empty()) {
        // Handle empty filename by setting everything to zero
        width = 0;
        height = 0;
        pixels.clear(); // Ensure the pixel data is empty

        return;
    }
    std::string texturePath = "textures/";
    std::string path = texturePath+filename+".ppm";
    auto [data, dimensions] = readPPM(path);
    width = dimensions.first;
    height = dimensions.second;
    pixels = std::move(data);
}

Colour Texture::getPixel(int u, int v) const {
    u = (u % width + width) % width;
    v = (v % height + height) % height;
    return pixels[v * width + u];
}


Colour Texture::sample(float u, float v) const {
    u = u - std::floor(u);  // Wrap UV to [0, 1)
    v = v - std::floor(v);

    float x = u * width;
    float y = v * height;

    int x0 = static_cast<int>(std::floor(x));
    int y0 = static_cast<int>(std::floor(y));
    int x1 = (x0 + 1) % width;
    int y1 = (y0 + 1) % height;

    // Bilinear interpolation
    float dx = x - x0;
    float dy = y - y0;

    Colour c00 = getPixel(x0, y0);
    Colour c10 = getPixel(x1, y0);
    Colour c01 = getPixel(x0, y1);
    Colour c11 = getPixel(x1, y1);

    return c00 * (1 - dx) * (1 - dy) +
           c10 * dx * (1 - dy) +
           c01 * (1 - dx) * dy +
           c11 * dx * dy;
}
