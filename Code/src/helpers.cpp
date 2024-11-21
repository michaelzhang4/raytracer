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

std::pair<std::vector<Colour>, std::pair<int, int>> readPPM(const std::string &filename) {
    std::ifstream inFile(filename, std::ios::in);
    if (!inFile) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return {};
    }

    std::string header;
    inFile >> header;
    if (header != "P3") {
        std::cerr << "Invalid PPM format (must be P3): " << filename << std::endl;
        return {};
    }

    int width, height, maxVal;
    inFile >> width >> height >> maxVal;
    if (maxVal != 255) {
        std::cerr << "Unsupported max colour value (must be 255): " << maxVal << std::endl;
        return {};
    }

    std::vector<Colour> pixels;
    pixels.reserve(width * height);

    int r, g, b;
    while (inFile >> r >> g >> b) {
        pixels.emplace_back(r, g, b);
    }

    return {pixels, {width, height}};
}

// Helper function to convert string to RenderMode
RenderMode stringToRenderMode(const std::string& mode) {
    if (mode == "binary") {
        return RenderMode::BINARY;
    } else if (mode == "phong") {
        return RenderMode::PHONG;
    } else if (mode == "pathtracer") {
        return RenderMode::PATH;
    } else {
        throw std::runtime_error("Unsupported render mode: " + mode);
    }
}

// Utility to generate random points
float randomFloat() {
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(rng);
}