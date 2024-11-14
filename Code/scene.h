#ifndef SCENE_H
#define SCENE_H

#include "common.h"
#include "camera.h"
#include "shapes.h"
#include "light.h"

// Scene class
class Scene {
public:
    Scene(const json& jsonData);
    void renderScene(std::vector<Colour>& pixels);
    void printSceneInfo();
    std::pair<int, int> sceneWidthHeight();
private:
    int nbounces;
    Camera camera;
    Colour backgroundColour;
    std::vector<std::shared_ptr<Shape>> shapes;
    std::vector<Light> lights;
    RenderMode renderMode;
};

#endif