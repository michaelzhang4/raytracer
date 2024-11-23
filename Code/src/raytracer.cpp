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
                pixelColour = Colour(255.0f, 0.0f, 0.0f);  // Red for hit objects
            } else {
                pixelColour = Colour(0.0f, 0.0f, 0.0f);  // Black otherwise
            }
            pixels[y * camera->width + x] = pixelColour;
        }
    }
}

float fresnel(const Vec3& I, const Vec3& N, float n1, float n2) {
    float cosI = I.dot(N);
    float etaI = n1;
    float etaT = n2;
    Vec3 normal = N;

    // Determine if the ray is entering or exiting
    if (cosI > 0.0f) {  // Ray is exiting the material
        std::swap(etaI, etaT);  // Swap the indices
        normal = -N;            // Flip the normal
        cosI = -cosI;           // Make cosI positive
    }

    // Compute sinT2 using Snell's Law
    float eta = etaI / etaT;
    float sinT2 = eta * eta * (1.0f - cosI * cosI);

    // Total internal reflection
    if (sinT2 > 1.0f) {
        return 1.0f;
    }

    float cosT = std::sqrt(1.0f - sinT2);

    // Compute Fresnel reflectance for perpendicular and parallel polarizations
    float rPerp = ((etaT * cosI) - (etaI * cosT)) / ((etaT * cosI) + (etaI * cosT) + 1e-6f);
    float rPara = ((etaI * cosI) - (etaT * cosT)) / ((etaI * cosI) + (etaT * cosT) + 1e-6f);

    // Average reflectance
    float reflectance = (rPerp * rPerp + rPara * rPara) * 0.5f;

    return std::clamp(reflectance, 0.0f, 1.0f);
}


float fresnelSchlick(float cosTheta, float n1, float n2) {
    float R0 = (n1 - n2) / (n1 + n2);
    R0 = R0 * R0;
    return R0 + (1.0f - R0) * pow(1.0f - cosTheta, 5.0f);
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

    Colour colour(0.0f, 0.0f, 0.0f);;



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
        float shadowFactor = 0.01f;

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
            shadowFactor = std::clamp(lightIntensity, 0.0f, 1.0f);
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
    Colour globalIllumination = textureDiffuseColor  * 0.4f;
    colour = colour + globalIllumination;

    if (material->isReflective || material->isRefractive) {
        Vec3 adjustedNormal = normal;
        float n1 = 1.0f, n2 = material->refractiveIndex;

        // Determine if the ray is entering or exiting the material
        if (ray.direction.dot(normal) > 0.0f) {  // Exiting the material
            adjustedNormal = -normal;  // Flip normal for correct reflection/refraction
        }

        float eta = n1 / n2;
            // Calculate the cosine of the incident angle
        float cosTheta = std::clamp(-ray.direction.dot(adjustedNormal), 0.0f, 1.0f);

        // Use Schlick's approximation for Fresnel reflectance
        float fresnelReflectance = fresnelSchlick(cosTheta, n1, n2);
        Colour reflectedColour(0, 0, 0);
        Colour refractedColour(0, 0, 0);

        // --- Reflection ---
        if (material->isReflective) {
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
                refractedColour = material->diffuseColor * traceRayRecursive(scene, Ray(hitPoint - adjustedNormal * 0.0001f, refractedDir), bounce + 1);
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

    // === Photon Emission Phase ===
    PhotonMap photonMap;
    int numPhotons = 100000;
    emitPhotons(scene, photonMap, numPhotons);


    // Optimise photon map, build k-d tree
    photonMap.build();

    // photonMap.printDebugInfo();

    #pragma omp parallel for
    for (int y = 0; y < camera->height; ++y) {
        for (int x = 0; x < camera->width; ++x) {
            // Pixel sampling / anti-aliasing
            Colour pixelColour = tracePixel(scene, x, y, photonMap);

            // Apply tone mapping to the final pixel before storage
            //ACES
            Vec3 hdrColor(pixelColour.r / 255.0f, pixelColour.g / 255.0f, pixelColour.b / 255.0f);

            Vec3 acesTonemapped = ACESFittedToneMap(hdrColor, exposure);

            // Convert Vec3 back to Colour (scaling to 0–255 range)
            pixelColour = Colour(
                static_cast<int>(acesTonemapped.x * 255.0f),
                static_cast<int>(acesTonemapped.y * 255.0f),
                static_cast<int>(acesTonemapped.z * 255.0f)
            );
            pixels[y * camera->width + x] = pixelColour;
        }
    }

    // Gamma correction
    // for (int y =0;y< camera->height;++y) {
    //     for (int x=0;x<camera->width; ++x) {
    //         pixels[y*camera->width + x] = gammaCorrect(pixels[y*camera->width + x]);
    //     }
    // }

    // // histogram equalisation
    // auto histogram = computeHistogram(pixels);
    // auto cdf = computeCDF(histogram);
    // applyHistogramEqualisation(pixels, exposure);

}

// Pixel sampling!
Colour PathTracer::tracePixel(const Scene& scene, int x, int y, const PhotonMap& photonMap) const {
    std::shared_ptr<Camera> camera = scene.getCamera();
    int samplesPerPixel = 30; // Number of samples per pixel
    Colour accumulatedColour(0, 0, 0);


    for (int i = 0; i < samplesPerPixel; ++i) {
        // Generate a random jitter within the pixel boundaries
        float jitterX = generateRandomNumber(-0.5f, 0.5f);
        float jitterY = generateRandomNumber(-0.5f, 0.5f);

        Vec3 rayDir = camera->getRayDirection(x + jitterX, y + jitterY);
        Ray jitteredRay(camera->position, rayDir);
        // Trace the jittered ray
        Colour sampleColour = traceRayRecursive(scene, jitteredRay, 0, photonMap);

        // Accumulate the colour
        accumulatedColour = accumulatedColour + sampleColour;
    }

    // Average the accumulated colour
    return accumulatedColour / static_cast<float>(samplesPerPixel);
}

Vec3 PathTracer::sampleHemisphere(const Vec3& normal) const {
    // Generate two random numbers in [0, 1]
    float u1 = generateRandomNumber(0.0f, 1.0f);
    float u2 = generateRandomNumber(0.0f, 1.0f);

    // Convert random numbers to spherical coordinates
    float theta = std::acos(std::sqrt(u1)); // Elevation angle
    float phi = 2.0f * M_PI * u2;           // Azimuth angle

    // Convert spherical coordinates to Cartesian coordinates
    float x = std::sin(theta) * std::cos(phi);
    float y = std::sin(theta) * std::sin(phi);
    float z = std::cos(theta);

    // Create the sample direction in tangent space
    Vec3 sample(x, y, z);

    // Align the sample direction with the given normal
    // Build an orthonormal basis around the normal
    Vec3 tangent, bitangent;
    if (std::abs(normal.x) > std::abs(normal.z)) {
        tangent = Vec3(-normal.y, normal.x, 0.0f).normalise();
    } else {
        tangent = Vec3(0.0f, -normal.z, normal.y).normalise();
    }
    bitangent = normal.cross(tangent);

    // Transform the sample to world space
    Vec3 worldSample = tangent * sample.x + bitangent * sample.y + normal * sample.z;

    return worldSample.normalise();
}


void PathTracer::emitPhotons(const Scene& scene, PhotonMap& photonMap, int numPhotons) const {
    for (int i = 0; i < numPhotons; ++i) {
        for (const auto& light : scene.getLights()) {
            // Assume light is an AreaLight for now
            auto areaLight = std::dynamic_pointer_cast<AreaLight>(light);
            if (!areaLight) continue;

            // Sample a random direction for the photon
            Vec3 photonDirection = sampleHemisphere(areaLight->getNormal());
            Vec3 photonOrigin = areaLight->samplePoint(); // Random point on the light source

            // Create a photon ray
            Ray photonRay(photonOrigin, photonDirection);

            // Set photon energy proportional to the light intensity
            Colour photonEnergy = light->intensity;//static_cast<float>(numPhotons);

            // Clamp photon energy to valid range
            photonEnergy.clamp();

            // Start photon tracing
            tracePhoton(scene, photonRay, photonEnergy, photonMap, 0);
        }
    }
}

Colour PathTracer::gatherCaustics(const PhotonMap& photonMap, const Vec3& position, float radius) const {
    auto photons = photonMap.query(position, radius);
    Colour caustics(0.0f, 0.0f, 0.0f);

    // Check if no photons were gathered
    if (photons.empty()) {
        return caustics; // Return zero colour if no photons are found
    }

    // Precompute normalisation factor
    float normalisationFactor = 1.0f / (M_PI * radius * radius);

    for (const auto& photon : photons) {
        float distanceSquared = (photon.position - position).length();
        distanceSquared = distanceSquared * distanceSquared;
        float weight = std::max(0.0f, 1.0f - distanceSquared / (radius * radius)); // Weight by proximity
        caustics = caustics + photon.energy * weight;
    }


    // Normalise the result by query area
    caustics = caustics * normalisationFactor;
    // std::cout << caustics.r << " " << caustics.g << " " << caustics.b << std::endl;

    // Clamp final colour values to prevent overflows
    caustics.clamp(); 

    // Limit the maximum contribution from caustics
    float maxIndirectIllumination = 0.01f; // Cap for indirect illumination
    caustics = caustics * maxIndirectIllumination;

    return caustics; // Normalize by query area
}


void PathTracer::tracePhoton(const Scene& scene, const Ray& ray, Colour energy, PhotonMap& photonMap, int depth) const {
    if (depth > scene.getBounces() || energy.belowThreshold()) return;

    Intersection intersection;
    if (!scene.intersect(ray, intersection)) return;

    const Vec3& hitPoint = intersection.hitPoint;
    const Vec3& normal = intersection.normal;
    auto material = intersection.shape->getMaterial();

    // Handle diffuse surfaces: store photons for caustics
    if (!material->isReflective && !material->isRefractive) {
        photonMap.storePhoton(hitPoint, ray.direction, energy);
        return;
    }

    // Handle reflective/refractive materials
    if (material->isReflective) {
        Vec3 reflectedDir = ray.direction - normal * 2.0f * ray.direction.dot(normal);
        Ray reflectedRay(hitPoint + normal * 1e-4f, reflectedDir);
        tracePhoton(scene, reflectedRay, energy * material->reflectivity, photonMap, depth + 1);
    }

    if (material->isRefractive) {
        Vec3 refractedDir;
        float eta = 1.0f / material->refractiveIndex;
        float cosI = -normal.dot(ray.direction);
        float sinT2 = eta * eta * (1.0f - cosI * cosI);

        if (sinT2 <= 1.0f) { // No total internal reflection
            float cosT = std::sqrt(1.0f - sinT2);
            refractedDir = ray.direction * eta + normal * (eta * cosI - cosT);
            Ray refractedRay(hitPoint - normal * 1e-4f, refractedDir);
            tracePhoton(scene, refractedRay, energy * (1.0f - material->reflectivity), photonMap, depth + 1);
        } else {
            // Total Internal Reflection
            Vec3 reflectedDir = ray.direction - normal * 2.0f * ray.direction.dot(normal);
            Ray reflectedRay(hitPoint + normal * 1e-4f, reflectedDir); // Offset to prevent self-intersection
            tracePhoton(scene, reflectedRay, energy, photonMap, depth + 1);
        }
    }
}

Colour PathTracer::traceRayRecursive(const Scene& scene, const Ray& ray, int bounce, const PhotonMap& photonMap) const {
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

    Colour colour = Colour(0.0f, 0.0f, 0.0f);



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
            const Colour& lightIntensity = light->intensity * 255.0f;

            // Number of samples per light (can be adjusted for better quality)
            int samples = 8;
            Colour lightContribution(0.0f, 0.0f, 0.0f);
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
    // Colour globalIllumination = textureDiffuseColor  * 0.25f;
    // colour = colour + globalIllumination;


    if (!material->isReflective && !material->isRefractive) {
        // Gather indirect light from photon map
        float searchRadius = 0.1f; // Adjust based on scene scale
        Colour indirectIllumination = gatherCaustics(photonMap, hitPoint, searchRadius);
        indirectIllumination.clamp(); // Ensure valid range

        // Add indirect illumination to final colour
        colour = colour + indirectIllumination;
        colour.clamp(); // Final clamping
    }


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
        bool entering = ray.direction.dot(normal) < 0.0f;
        Vec3 adjustedNormal = entering ? normal : -normal;
        float n1 = entering ? 1.0f : material->refractiveIndex;
        float n2 = entering ? material->refractiveIndex : 1.0f;
        float eta = n1 / n2;
        float cosI = -adjustedNormal.dot(ray.direction);
        // Vec3 adjustedNormal = normal;
        // float n1 = 1.0f, n2 = material->refractiveIndex;

        // // Determine if the ray is entering or exiting the material
        // if (ray.direction.dot(normal) > 0.0f) {  // Exiting the material
        //     std::swap(n1, n2);  // Swap indices
        //     adjustedNormal = -normal;  // Flip normal for correct reflection/refraction
        // }

        // float eta = n1 / n2;
        float fresnelReflectance = 0.0f;
        Colour reflectedColour(0, 0, 0);
        Colour refractedColour(0, 0, 0);

        // --- Reflection ---
        if (material->isReflective) {
            int numSamples = 4;
            // Ensure that roughness is between 0.0f and 1.0f
            float roughness = std::sqrt(2.0f / (material->specularExponent + 2.0f));
            roughness = std::clamp(roughness, 0.0f, 1.0f);


            // Sample multiple directions for indirect light (brdf)
            for (int i = 0; i < numSamples; i++) {
                // Sample the GGX halfway vector
                Vec3 halfVector = BRDF::sampleGGX(adjustedNormal, roughness);

                Vec3 reflectedDir = ray.direction - adjustedNormal * 2.0f * ray.direction.dot(adjustedNormal);

                // Trace the reflected ray
                Colour sampleColour = traceRayRecursive(scene, Ray(hitPoint + adjustedNormal * 0.0001f, reflectedDir), bounce + 1, photonMap);

                // Evaluate the Cook-Torrance BRDF
                Colour F0 = material->specularColor;  // Base reflectance for non-metals
                Colour brdf = BRDF::CookTorrance(
                    reflectedDir,            // Light direction
                    -ray.direction,          // View direction
                    adjustedNormal,          // Surface normal
                    halfVector,              // Halfway vector
                    material->diffuseColor / 255.0f,  // Base diffuse colour (converted to [0, 1] range)
                    F0,                      // Fresnel reflectance at normal incidence
                    roughness                // Surface roughness
                );

                // Calculate the GGX PDF for the sampled direction
                float pdf = BRDF::GGX_PDF(adjustedNormal, halfVector, roughness);

                // Compute the weight
                float NdotL = std::max(adjustedNormal.dot(reflectedDir), 0.0f);
                Colour weight = (brdf * NdotL) / pdf;

                // Weight the contribution using the BRDF and PDF
                reflectedColour = reflectedColour + sampleColour *  weight;
            }

            // Average the contributions over the number of samples
            reflectedColour = reflectedColour / float(numSamples);
        }

        // float cosI = -adjustedNormal.dot(ray.direction);

        // --- Refraction ---
        if (material->isRefractive) {
            float sinT2 = eta * eta * (1.0f - cosI * cosI);

            if (sinT2 <= 1.0f) {  // No total internal reflection
                float cosT = std::sqrt(1.0f - sinT2);
                Vec3 refractedDir = ray.direction * eta + adjustedNormal * (eta * cosI - cosT);
                refractedDir.normalise();

                refractedColour = traceRayRecursive(scene, Ray(hitPoint - adjustedNormal * 0.0001f, refractedDir), bounce + 1, photonMap);

                // Multiply by material's transmittance (diffuse color)
                Colour transmittance = material->diffuseColor / 255.0f;
                refractedColour = refractedColour * transmittance;

                // Schlick approximation
                float r0 = (n1 - n2) / (n1 + n2);
                r0 = r0 * r0;
                fresnelReflectance = r0 + (1.0f - r0) * std::pow(1.0f - cosI, 5.0f);
                fresnelReflectance = std::clamp(fresnelReflectance, 0.0f, 1.0f);
            } else {
                fresnelReflectance = 1.0f;  // Total internal reflection
            }
        } else if (material->isReflective) {
            // Schlick approximation
            float r0 = (n1 - n2) / (n1 + n2);
            r0 = r0 * r0;
            fresnelReflectance = r0 + (1.0f - r0) * std::pow(1.0f - std::abs(cosI), 5.0f);
            fresnelReflectance = std::max(fresnelReflectance, 0.05f); // Clamp Fresnel reflectance
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
