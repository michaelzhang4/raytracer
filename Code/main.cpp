#include "helpers.h"
#include "scene.h"
#include "parser.h"

int main() {
    json jsonData;
    parse("../TestSuite/scene.json", jsonData);

    Scene scene(jsonData);

    scene.printSceneInfo();

    auto [width, height] = scene.sceneWidthHeight();

    // // Prepare the pixel buffer
    std::vector<Colour> pixels(width * height);

    // // Render the scene
    scene.renderScene(pixels);

    // // Save the result to a PPM file
    std::string filename = "output.ppm";
    writePPM(filename, pixels, width, height);

    std::cout << "Rendered image saved to " << filename << std::endl;
    return 0;
}
