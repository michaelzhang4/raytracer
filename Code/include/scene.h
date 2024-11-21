#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "shapes.h"
#include "raytracer.h"
#include "helpers.h"
#include "parser.h"
#include "tonemap.h"
#include "bvh.h"

// Scene class
class Scene {
public:
    Scene(const json& jsonData);
    void renderScene(std::vector<Colour>& pixels);
    void printSceneInfo();
    std::pair<int, int> sceneWidthHeight();
    std::shared_ptr<Camera> getCamera() const { return camera; }
    const Colour& getBackgroundColour() const { return backgroundColour; }
    const std::vector<std::shared_ptr<Shape>>& getShapes() const { return shapes; }
    const std::vector<Light>& getLights() const { return lights; }
    const RenderMode& getRenderMode() const { return renderMode; }
    int getBounces() const { return nbounces; }
    float getExposure() const { return camera->exposure; }
    bool intersect(const Ray& ray, Intersection& nearestIntersection) const;
private:
    int nbounces;
    std::shared_ptr<Camera> camera;
    Colour backgroundColour;
    std::vector<std::shared_ptr<Shape>> shapes;
    std::vector<Light> lights;
    RenderMode renderMode;
    BVH bvh;
};

#endif