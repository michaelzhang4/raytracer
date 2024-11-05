#include "raytracer.h"

int main() {
    json jsonData;
    parse("../TestSuite/binary_primitves.json", jsonData);

    Scene scene(jsonData);

    scene.printSceneInfo();

    // // Prepare the pixel buffer
    // std::vector<Colour> pixels(width * height);

    // // Render the scene
    // renderScene(pixels, width, height, camera);

    // // Save the result to a PPM file
    // std::string filename = "output.ppm";
    // writePPM(filename, pixels, width, height);

    // std::cout << "Rendered image saved to " << filename << std::endl;
    return 0;
}
