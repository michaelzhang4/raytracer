#include "texture.h"

// Texture::Texture(const std::string& filename) {
//     // Example of loading a PPM file (Assume PPM::readPPM function exists)
//     auto [data, dimensions] = PPM::readPPM(filename);
//     width = dimensions.x();
//     height = dimensions.y();
//     pixels = std::move(data);
// }

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
