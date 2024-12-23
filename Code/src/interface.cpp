#include "interface.h"

void textInterface() {
    std::string input;
    std::string jsonPath = "jsons/";
    std::string outputPath = "../TestSuite/";
    std::cout << "Type 'exit' to leave";
    while (true) {
        std::cout << std::endl << "Enter the json file you wish to render: " << std::endl;
        std::cin >> input;
        if (input=="exit") {
            break;
        }
        json jsonData;

        if (parse(jsonPath+input+".json", jsonData) == -1) {
            std::cout << "Error parsing "+input+".json. Please try again" << std::endl;
            continue;
        };

        Scene scene(jsonData);
        // scene.printSceneInfo();

        auto [width, height] = scene.sceneWidthHeight();

        // Prepare the pixel buffer
        std::vector<Colour> pixels(width * height);


        auto start = std::chrono::high_resolution_clock::now();

        // Render the scene
        scene.renderScene(pixels);


        auto end = std::chrono::high_resolution_clock::now();

        // Save the result to a PPM file
        std::string filename = outputPath+input+".ppm";
        writePPM(filename, pixels, width, height);

        std::cout << "Rendered image saved to " << filename << std::endl;
        
        std::chrono::duration<double> elapsed = end - start;

        std::cout << "Render time: " << elapsed.count() << " s" << std::endl;

    }
}