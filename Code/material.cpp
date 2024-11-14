#include "material.h"

// Default constructor
Material::Material()
    : kd(0.8f), ks(0.2f), specularExponent(10), diffuseColor({255, 255, 255}),
      specularColor({255, 255, 255}), isReflective(false), reflectivity(0.0f),
      isRefractive(false), refractiveIndex(1.0f) {}

// Parameterized constructor
Material::Material(float kd, float ks, int specExp, const Colour& diffuse, const Colour& specular,
                   bool reflective, float reflectivity, bool refractive, float refractiveIndex)
    : kd(kd), ks(ks), specularExponent(specExp), diffuseColor(diffuse),
      specularColor(specular), isReflective(reflective), reflectivity(reflectivity),
      isRefractive(refractive), refractiveIndex(refractiveIndex) {}

// Print material info (for debugging)
void Material::printMaterialInfo() const {
    std::cout << "Material Info:" << std::endl;
    std::cout << "Diffuse Color: (" << diffuseColor.r << ", " << diffuseColor.g << ", " << diffuseColor.b << ")" << std::endl;
    std::cout << "Specular Color: (" << specularColor.r << ", " << specularColor.g << ", " << specularColor.b << ")" << std::endl;
    std::cout << "kd (Diffuse coefficient): " << kd << std::endl;
    std::cout << "ks (Specular coefficient): " << ks << std::endl;
    std::cout << "Specular Exponent: " << specularExponent << std::endl;
    std::cout << "Reflective: " << (isReflective ? "Yes" : "No") << ", Reflectivity: " << reflectivity << std::endl;
    std::cout << "Refractive: " << (isRefractive ? "Yes" : "No") << ", Refractive Index: " << refractiveIndex << std::endl;
}
