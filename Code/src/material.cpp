#include "material.h"

// Default constructor
Material::Material()
    : kd(0.8f), ks(0.2f), specularExponent(2), diffuseColor(Colour(255.0f, 0.0f, 0.0f)),
      specularColor(Colour(255.0f, 0.0f, 0.0f)), isReflective(false), reflectivity(0.0f),
      isRefractive(false), refractiveIndex(0.0f), roughness(0.3f), texture(nullptr) {}

// Parameterized constructor
Material::Material(float kd, float ks, int specExp, const Colour& diffuse, const Colour& specular,
                   bool reflective, float reflectivity, bool refractive, float refractiveIndex,  float roughness, std::string texturePath)
    : kd(kd), ks(ks), specularExponent(specExp), diffuseColor(diffuse),
      specularColor(specular), isReflective(reflective), reflectivity(reflectivity),
      isRefractive(refractive), refractiveIndex(refractiveIndex), roughness(roughness){
      if (!texturePath.empty()) {
          texture = std::make_shared<Texture>(texturePath);  // Create a shared pointer
        } else {
          texture = nullptr;  // No texture provided
        }
      }

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
