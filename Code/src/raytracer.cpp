#include "raytracer.h"
#include "scene.h"


void BinaryTracer::renderScene(const Scene& scene, std::vector<Colour>& pixels) const {
    std::shared_ptr<Camera> camera = scene.getCamera();
    const auto& shapes = scene.getShapes();
    const Colour& backgroundColour = scene.getBackgroundColour();

    // Resize the pixel buffer based on camera dimensions
    pixels.resize(camera->width * camera->height);
    
    // Parallel rendering loop
    #pragma omp parallel for
    for (int y = 0; y < camera->height; ++y) {
        for (int x = 0; x < camera->width; ++x) {
            Ray ray(camera->position, camera->getRayDirection(x, y).normalise());
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
            pixels[y * camera->width + x] = pixelColour;
        }
    }
}

float fresnel(const Vec3& I, const Vec3& N, float eta) {
    float cosI = std::clamp(I.dot(N), -1.0f, 1.0f);  // Dot product of incident ray and normal
    float sinT2 = eta * eta * (1.0f - cosI * cosI);  // Snell's law to calculate sin^2(theta_t)
    if (cosI < 0.0f) {
        cosI = -cosI;  // Flip if the normal is in the wrong direction
    }
    if (sinT2 > 1.0f) {
        // Total internal reflection
        return 1.0f;
    }

    float cosT = std::sqrt(1.0f - sinT2);  // cos(theta_t)

    float rOrth = (eta * cosI - cosT) / (eta * cosI + cosT + 1e-6);  // Reflectance for perpendicular polarisation
    float rPara = (cosI - eta * cosT) / (cosI + eta * cosT + 1e-6);  // Reflectance for parallel polarisation

    return std::clamp((rOrth * rOrth + rPara * rPara) * 0.5f, 0.0f, 1.0f);
}

void PhongTracer::renderScene(const Scene& scene, std::vector<Colour>& pixels) const {
    std::shared_ptr<Camera> camera = scene.getCamera();
    float exposure = scene.getExposure();

    pixels.resize(camera->width * camera->height);

    #pragma omp parallel for
    for (int y = 0; y < camera->height; ++y) {
        for (int x = 0; x < camera->width; ++x) {
            Ray cameraRay = Ray(camera->position, camera->getRayDirection(x, y));

            // Compute pixel colour using recursive ray tracing
            Colour pixelColour = traceRayRecursive(scene, cameraRay, 0);

            // Apply tone mapping to the final pixel before storage
            pixelColour = linearToneMap(pixelColour, exposure);
            pixels[y * camera->width + x] = pixelColour;
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
    const auto lights = scene.getLights();
    std::shared_ptr<Camera> camera = scene.getCamera();

    Intersection nearestIntersection;
    nearestIntersection.t = std::numeric_limits<float>::max();
    nearestIntersection.hit = false;


    // Find nearest intersection using BVH
    if (!scene.intersect(ray, nearestIntersection)) {
        return backgroundColour; // No intersection
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
        Vec3 lightDir = (light->position - hitPoint).normalise();

        // Shadow ray
        Ray shadowRay(hitPoint + normal * 0.00001f, lightDir);
        bool inShadow = false;

        // Calculate distance to light
        float lightDistance = (light->position - hitPoint).length();
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
            float lightIntensity = (light->intensity.r + light->intensity.g + light->intensity.b) / 3.0f;
            shadowFactor = std::clamp(lightIntensity / 255.0f, 0.0f, 1.0f);
        }

        // Diffuse lighting
        float diffuseIntensity = std::max(0.0f, normal.dot(lightDir));
        Colour diffuse = textureDiffuseColor * diffuseIntensity * material->kd;
        
        // Specular lighting
        Vec3 viewDir = (camera->position - hitPoint).normalise();
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



void PathTracer::renderScene(const Scene& scene, std::vector<Colour>& pixels) const {
    std::shared_ptr<Camera> camera = scene.getCamera();
    float exposure = scene.getExposure();

    pixels.resize(camera->width * camera->height);

    #pragma omp parallel for
    for (int y = 0; y < camera->height; ++y) {
        for (int x = 0; x < camera->width; ++x) {
            // Pixel sampling / anti-aliasing
            Colour pixelColour = tracePixel(scene, x, y);

            // Apply tone mapping to the final pixel before storage
            pixelColour = linearToneMap(pixelColour, exposure);
            pixels[y * camera->width + x] = pixelColour;
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

// Pixel sampling!
Colour PathTracer::tracePixel(const Scene& scene, int x, int y) const {
    std::shared_ptr<Camera> camera = scene.getCamera();
    int samplesPerPixel = 8; // Number of samples per pixel
    Colour accumulatedColour = {0, 0, 0};

    for (int i = 0; i < samplesPerPixel; ++i) {
        // Generate a random jitter within the pixel boundaries
        float jitterX = generateRandomNumber(-0.5f, 0.5f);
        float jitterY = generateRandomNumber(-0.5f, 0.5f);

        Vec3 rayDir = camera->getRayDirection(x + jitterX, y + jitterY);
        Ray jitteredRay(camera->position, rayDir);
        // Trace the jittered ray
        Colour sampleColour = traceRayRecursive(scene, jitteredRay, 0);

        // Accumulate the colour
        accumulatedColour = accumulatedColour + sampleColour;
    }

    // Average the accumulated colour
    return accumulatedColour / static_cast<float>(samplesPerPixel);
}


Colour PathTracer::traceRayRecursive(const Scene& scene, const Ray& ray, int bounce) const {
    const int bounceCount = scene.getBounces();
    const Colour& backgroundColour = scene.getBackgroundColour();

    if (bounce > bounceCount) {
        return backgroundColour;
    }

    const auto& shapes = scene.getShapes();
    const auto& lights = scene.getLights();
    std::shared_ptr<Camera> camera = scene.getCamera();

    Intersection nearestIntersection;
    nearestIntersection.t = std::numeric_limits<float>::max();
    nearestIntersection.hit = false;


    // Find nearest intersection using BVH
    if (!scene.intersect(ray, nearestIntersection)) {
        return backgroundColour; // No intersection
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

    // Iterate over the lights in the scene
    for (const auto& light : lights) {
        // Attempt to cast the light to an AreaLight
        auto areaLight = std::dynamic_pointer_cast<AreaLight>(light);

        if (areaLight) {
            // Using built-in functions of AreaLight
            const Colour& lightIntensity = light->intensity;

            // Number of samples per light (can be adjusted for better quality)
            int samples = 8;
            Colour lightContribution = {0, 0, 0};
            float lightArea = areaLight->width * areaLight->height;

            // Sample light multiple times for area-based lighting
            for (int i = 0; i < samples; ++i) {
                // Generate a random sample point on the light surface using the built-in samplePoint function
                Vec3 lightSamplePoint = areaLight->samplePoint();

                Vec3 lightDir = (lightSamplePoint - hitPoint).normalise();
                float lightDistance = (lightSamplePoint - hitPoint).length();

                // Shadow ray with small offset to avoid self-intersection
                const float epsilon = 1e-4f;
                Ray shadowRay(hitPoint + normal * epsilon, lightDir);

                // Check for occlusion using the shadow ray
                bool inShadow = false;
                // Check for occlusion
                for (const auto& shape : shapes) {
                    Intersection shadowIntersection;
                    if (shape->intersect(Ray(shadowRay.origin, shadowRay.direction), shadowIntersection) && shadowIntersection.t > 0.0001f && shadowIntersection.t < lightDistance) {
                        inShadow = true;
                        break;
                    }
                }

                if (!inShadow) {
                    // Calculate diffuse contribution
                    float diffuseFactor = std::max(0.0f, normal.dot(lightDir));
                    Colour diffuseContribution = textureDiffuseColor * diffuseFactor * lightIntensity;

                    // Calculate specular contribution
                    Vec3 viewDir = (camera->position - hitPoint).normalise(); // Calculate view direction
                    Vec3 reflectDir = (normal * (2.0f * normal.dot(lightDir)) - lightDir).normalise();
                    float specularFactor = std::pow(std::max(0.0f, viewDir.dot(reflectDir)), material->specularExponent);
                    Colour specularContribution = material->specularColor * specularFactor * lightIntensity;

                    // Combine diffuse and specular contributions
                    Colour sampleContribution = diffuseContribution + specularContribution;

                    // Normalize by light area and add to the accumulated light contribution
                    lightContribution = lightContribution + sampleContribution / lightArea;
                }
            }

            // Average the contribution over all samples
            lightContribution = lightContribution / static_cast<float>(samples);

            // Add to the final colour
            colour = colour + lightContribution;
        }
    }


    // Adjust illumination of the scene (ambient lighting)
    Colour globalIllumination = textureDiffuseColor  * 0.25f;
    colour = colour + globalIllumination;

    // === Russian Roulette for Indirect Rays ===
    if (bounce > 2) { // Apply after 2 bounces
        // Estimate the maximum contribution of this ray
        float maxReflectance = std::max(material->reflectivity, material->kd);

        // Use a random value to terminate the path probabilistically
        float terminateProbability = std::clamp(maxReflectance, 0.1f, 0.95f); // Avoid too low or high probabilities
        if (generateRandomNumber(0.0f, 1.0f) > terminateProbability) {
            return colour; // Terminate this path
        }

        // Compensate for the reduced probability (ensures energy conservation)
        colour = colour / terminateProbability;
    }

    // Indirect light
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
            int numSamples = 4;
            float roughness = std::sqrt(2.0f / (material->specularExponent + 2.0f));

            // Sample multiple directions for indirect light (brdf)
            for (int i = 0; i < numSamples; i++) {
                // Sample the GGX halfway vector
                Vec3 halfVector = BRDF::sampleGGX(adjustedNormal, roughness);

                // Compute the reflected direction using the sampled halfway vector
                Vec3 reflectedDir = ray.direction - adjustedNormal * 2.0f * ray.direction.dot(adjustedNormal);


                // Trace the reflected ray
                Colour sampleColour = traceRayRecursive(scene, Ray(hitPoint + adjustedNormal * 0.0001f, reflectedDir), bounce + 1);

                // Evaluate the Cook-Torrance BRDF
                Colour F0 = Colour(10, 10, 10);  // Base reflectance for non-metals
                Colour brdf = BRDF::CookTorrance(
                    reflectedDir,            // Light direction
                    -ray.direction,          // View direction
                    adjustedNormal,          // Surface normal
                    halfVector,              // Halfway vector
                    material->diffuseColor / 255.0f,  // Base diffuse colour (converted to [0, 1] range)
                    F0,                      // Fresnel reflectance at normal incidence
                    roughness                // Surface roughness
                );
                float D = BRDF::GGX_D(adjustedNormal, halfVector, roughness);
                // Calculate the GGX PDF for the sampled direction
                float pdf = BRDF::GGX_PDF(adjustedNormal, halfVector, roughness);

                float weight = (D * adjustedNormal.dot(halfVector)) / std::max(pdf, 0.001f);
                weight = std::min(weight, 1.0f); // Ensure the sum doesn't exceed 1

                // Weight the contribution using the BRDF and PDF
                reflectedColour = reflectedColour + sampleColour *  weight + brdf;
            }

            // Average the contributions over the number of samples
            reflectedColour = reflectedColour / float(numSamples);
        }

        float cosI = -adjustedNormal.dot(ray.direction);

        // --- Refraction ---
        if (material->isRefractive) {
            float sinT2 = eta * eta * (1.0f - cosI * cosI);

            if (sinT2 <= 1.0f) {  // No total internal reflection
                float cosT = std::sqrt(1.0f - sinT2);
                Vec3 refractedDir = ray.direction * eta + adjustedNormal * (eta * cosI - cosT);
                refractedColour = traceRayRecursive(scene, Ray(hitPoint - adjustedNormal * 0.0001f, refractedDir), bounce + 1);
            } else {
                fresnelReflectance = 1.0f;  // Total internal reflection: fully reflective
            }
        }
        // Schlick approximation
        float r0 = (n1 - n2) / (n1 + n2);
        r0 = r0 * r0;
        fresnelReflectance = r0 + (1.0f - r0) * std::pow(1.0f - std::abs(cosI), 5.0f);
        fresnelReflectance = std::max(fresnelReflectance, 0.05f); // Clamp Fresnel reflectance

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
