#ifndef MATERIAL_H
#define MATERIAL_H

#include "common.h"

struct Material {
    float kd;  // Diffuse coefficient
    float ks;  // Specular coefficient
    int specularExponent;  // Specular exponent for shininess
    Colour diffuseColor;
    Colour specularColor;
    bool isReflective;
    float reflectivity;  // Reflectivity percentage
    bool isRefractive;
    float refractiveIndex;

    // Default constructor
    Material();

    // Parameterized constructor
    Material(float kd, float ks, int specularExponent, const Colour& diffuse, const Colour& specular, 
             bool reflective, float reflectivity, bool refractive, float refrIndex);
    // Colour getDiffuseColor(Point2f uv) const;
    // Debugging utility
    void printMaterialInfo() const;
};

#endif // MATERIAL_H
