#include "raytracer.h"
#include "scene.h"


void BinaryTracer::renderScene(const Scene& scene, std::vector<Colour>& pixels) const {
    const Camera& camera = scene.getCamera();
    const auto& shapes = scene.getShapes();
    const Colour& backgroundColour = scene.getBackgroundColour();

    // Resize the pixel buffer based on camera dimensions
    pixels.resize(camera.width * camera.height);
    
    // Parallel rendering loop
    #pragma omp parallel for
    for (int y = 0; y < camera.height; ++y) {
        for (int x = 0; x < camera.width; ++x) {
            Ray ray(camera.position, camera.getRayDirection(x, y).normalise());
            Colour pixelColour = backgroundColour;
            float nearestT = std::numeric_limits<float>::max(); // Nearest intersection distance
            std::shared_ptr<Shape> hitObject = nullptr;

            // Check intersection with all shapes
            for (const auto& shape : shapes) {
                float t = 0.0f;
                Intersection nearestIntersection;
                if (shape->intersect(ray, nearestIntersection)) {
                    if (t < nearestT) {
                        nearestT = t;
                        hitObject = shape;
                    }
                }
            }
            // Binary mode: set red if any object is hit
            if (hitObject) {
                pixelColour = Colour(255, 0, 0);  // Red for hit objects
            } else {
                pixelColour = Colour(0, 0, 0);  // Black otherwise
            }
            pixels[y * camera.width + x] = pixelColour;
        }
    }
}

float fresnel(const Vec3& I, const Vec3& N, float eta) {
    float cosI = std::clamp(-1.0f, 1.0f, I.dot(N));  // Dot product of incident ray and normal
    float sinT2 = eta * eta * (1.0f - cosI * cosI);  // Snell's law to calculate sin^2(theta_t)
    
    if (sinT2 > 1.0f) {
        // Total internal reflection
        return 1.0f;
    }

    float cosT = std::sqrt(1.0f - sinT2);  // cos(theta_t)

    float rOrth = (eta * cosI - cosT) / (eta * cosI + cosT);  // Reflectance for perpendicular polarisation
    float rPara = (cosI - eta * cosT) / (cosI + eta * cosT);  // Reflectance for parallel polarisation

    return (rOrth * rOrth + rPara * rPara) * 0.5f;  // Average reflectance
}

void PhongTracer::renderScene(const Scene& scene, std::vector<Colour>& pixels) const {
    const Camera& camera = scene.getCamera();
    float exposure = scene.getExposure();

    pixels.resize(camera.width * camera.height);

    #pragma omp parallel for
    for (int y = 0; y < camera.height; ++y) {
        for (int x = 0; x < camera.width; ++x) {
            Ray cameraRay = Ray(camera.position, camera.getRayDirection(x, y));

            // Compute pixel colour using recursive ray tracing
            Colour pixelColour = traceRayRecursive(scene, cameraRay, 0);

            // Apply tone mapping to the final pixel before storage
            pixelColour = linearToneMap(pixelColour, exposure);
            pixels[y * camera.width + x] = pixelColour;
        }
    }

    // Gamma correction
    // for (int y =0;y< camera.height;++y) {
    //     for (int x=0;x<camera.width; ++x) {
    //         pixels[y*camera.width + x] = gammaCorrect(pixels[y*camera.width + x]);
    //     }
    // }

    // // histogram equalisation
    // auto histogram = computeHistogram(pixels);
    // auto cdf = computeCDF(histogram);
    // applyHistogramEqualisation(pixels, exposure);

}

Colour PhongTracer::traceRayRecursive(const Scene& scene, const Ray& ray, int bounce) const {
    const int bounceCount = scene.getBounces();
    const Colour& backgroundColour = scene.getBackgroundColour();

    if (bounce > bounceCount) {
        return backgroundColour;
    }

    const auto& shapes = scene.getShapes();
    const auto& lights = scene.getLights();
    const Camera& camera = scene.getCamera();

    Intersection nearestIntersection;
    nearestIntersection.t = std::numeric_limits<float>::max();
    nearestIntersection.hit = false;

    // Find nearest intersection
    for (const auto& shape : shapes) {
        Intersection tempIntersection;
        if (shape->intersect(ray, tempIntersection) && tempIntersection.t < nearestIntersection.t) {
            nearestIntersection = tempIntersection;
        }
    }

    if (!nearestIntersection.hit) {
        return backgroundColour;
    }

    const Vec3& hitPoint = nearestIntersection.hitPoint;
    const Vec3& normal = nearestIntersection.normal;

    std::shared_ptr<Material> material = nearestIntersection.shape->getMaterial();

    Colour colour = {0, 0, 0};



    // Sample texture if the material has one
    Colour textureDiffuseColor = material->diffuseColor;
    if (material->texture && material->texture->width > 0) {
        // Get texture coordinates
        std::pair<float, float> uv = nearestIntersection.shape->getUV(hitPoint);
        float u = uv.first, v = uv.second;
        textureDiffuseColor = material->texture->sample(u, v);
    }
    

    // Light contributions
    for (const auto& light : lights) {
        Vec3 lightDir = (light.position - hitPoint).normalise();

        // Shadow ray
        Ray shadowRay(hitPoint + normal * 0.00001f, lightDir);
        bool inShadow = false;

        // Calculate distance to light
        float lightDistance = (light.position - hitPoint).length();
        float shadowFactor = 0.1f;

        // Check for occlusion
        for (const auto& shape : shapes) {
            Intersection shadowIntersection;
            if (shape->intersect(Ray(shadowRay.origin, shadowRay.direction), shadowIntersection) && shadowIntersection.t > 0.0001f && shadowIntersection.t < lightDistance) {
                inShadow = true;
                break;
            }
        }

        if (!inShadow) {
            shadowFactor = 1.0f;
        }

        // Adjust ambient light based on whether the point is in shadow
        // Diffuse lighting
        float diffuseIntensity = std::max(0.0f, normal.dot(lightDir));
        Colour diffuse = textureDiffuseColor * diffuseIntensity * material->kd;
        
        // Specular lighting
        Vec3 viewDir = (camera.position - hitPoint).normalise();
        Vec3 halfVector = (lightDir + viewDir).normalise();
        float specularIntensity = std::pow(std::max(0.0f, normal.dot(halfVector)), material->specularExponent);
        
        // specular constant
        Colour specular = material->specularColor * specularIntensity * material->ks * 0.4f;

        colour = colour + (diffuse + specular) * shadowFactor;   
    }

    // Adjust illumination of the scene (ambient lighting)
    Colour globalIllumination = textureDiffuseColor  * 0.25f;
    colour = colour + globalIllumination;

    if (material->isReflective || material->isRefractive) {
        Vec3 adjustedNormal = normal;
        float n1 = 1.0f, n2 = material->refractiveIndex;

        // Determine if the ray is entering or exiting the material
        if (ray.direction.dot(normal) > 0.0f) {  // Exiting the material
            std::swap(n1, n2);  // Swap indices
            adjustedNormal = -normal;  // Flip normal for correct reflection/refraction
        }

        float eta = n1 / n2;
        float fresnelReflectance = 0.0f;
        Colour reflectedColour(0, 0, 0);
        Colour refractedColour(0, 0, 0);

        // --- Reflection ---
        if (material->isReflective) {
            fresnelReflectance = fresnel(ray.direction, adjustedNormal, eta);
            Vec3 reflectedDir = ray.direction - adjustedNormal * 2.0f * ray.direction.dot(adjustedNormal);
            reflectedColour = traceRayRecursive(scene, Ray(hitPoint + adjustedNormal * 0.0001f, reflectedDir), bounce + 1);
        }

        // --- Refraction ---
        if (material->isRefractive) {
            float cosI = -adjustedNormal.dot(ray.direction);
            float sinT2 = eta * eta * (1.0f - cosI * cosI);

            if (sinT2 <= 1.0f) {  // No total internal reflection
                float cosT = std::sqrt(1.0f - sinT2);
                Vec3 refractedDir = ray.direction * eta + adjustedNormal * (eta * cosI - cosT);
                refractedColour = traceRayRecursive(scene, Ray(hitPoint - adjustedNormal * 0.0001f, refractedDir), bounce + 1);
            } else {
                fresnelReflectance = 1.0f;  // Total internal reflection: fully reflective
            }
        }

        // --- Blend reflection and refraction ---
        if (material->isReflective && material->isRefractive) {
            colour = reflectedColour * fresnelReflectance + refractedColour * (1.0f - fresnelReflectance);
        } else if (material->isReflective) {
            colour = reflectedColour;  // Pure reflection
        } else if (material->isRefractive) {
            colour = refractedColour;  // Pure refraction
        }
    }
    return colour;
}