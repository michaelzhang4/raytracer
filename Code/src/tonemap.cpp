#include "tonemap.h"

Colour linearToneMap(const Colour& hdrColour, float exposure) {
    Colour scaled = hdrColour * exposure;
    scaled.clamp(); // Apply exposure and clamp
    return scaled;
}

Vec3 ACESFittedToneMap(const Vec3& color, float exposure) {
    const float A = 0.0245786f;
    const float B = 0.000090537f;
    const float C = 0.983729f;
    const float D = 0.432951f;
    const float E = 0.238081f;

    // Apply exposure
    Vec3 hdrColor = color * exposure;

    // Perform ACES tone mapping
    Vec3 tonemappedColor = (hdrColor * (hdrColor + Vec3(A)) - Vec3(B)) / (hdrColor * (hdrColor * C + Vec3(D)) + Vec3(E));

    // Clamp the output to [0, 1] range
    return Vec3(
        std::clamp(tonemappedColor.x, 0.0f, 1.0f),
        std::clamp(tonemappedColor.y, 0.0f, 1.0f),
        std::clamp(tonemappedColor.z, 0.0f, 1.0f)
    );
}

// Reinhard's Global Tone Mapping Function
Colour reinhardToneMap(const Colour& hdrColour, float exposure) {
    //Convert integer Colour to floating-point [0.0, 1.0]
    float r_f = static_cast<float>(hdrColour.r) / 255.0f;
    float g_f = static_cast<float>(hdrColour.g) / 255.0f;
    float b_f = static_cast<float>(hdrColour.b) / 255.0f;

    // Compute luminance Y using Rec. 709 coefficients
    float Y = 0.2126f * r_f + 0.7152f * g_f + 0.0722f * b_f;

    // Apply exposure and Reinhard's mapping
    float Y_scaled = Y * exposure;
    float Y_mapped = Y_scaled / (1.0f + Y_scaled);

    // Compute scaling factor to maintain color ratios
    float scale = (Y > 0.0f) ? (Y_mapped / Y) : 0.0f;

    // Scale RGB channels
    float r_mapped = r_f * scale;
    float g_mapped = g_f * scale;
    float b_mapped = b_f * scale;

    // Convert back to integer Colour with clamping
    int r_final = static_cast<int>(std::round(r_mapped * 255.0f));
    int g_final = static_cast<int>(std::round(g_mapped * 255.0f));
    int b_final = static_cast<int>(std::round(b_mapped * 255.0f));

    // Clamp the results to [0, 255]
    r_final = std::clamp(r_final, 0, 255);
    g_final = std::clamp(g_final, 0, 255);
    b_final = std::clamp(b_final, 0, 255);

    // Return the mapped Colour
    return Colour(r_final, g_final, b_final);
}

// Gamma correction function for integer Colour
Colour gammaCorrect(const Colour& linearColour, float gamma) {
    //Normalize the color channels to [0.0, 1.0]
    float r_norm = static_cast<float>(std::max(linearColour.r, 0)) / 255.0f;
    float g_norm = static_cast<float>(std::max(linearColour.g, 0)) / 255.0f;
    float b_norm = static_cast<float>(std::max(linearColour.b, 0)) / 255.0f;

    //Apply gamma correction
    float r_gamma = std::pow(r_norm, 1.0f / gamma);
    float g_gamma = std::pow(g_norm, 1.0f / gamma);
    float b_gamma = std::pow(b_norm, 1.0f / gamma);

    //Denormalize back to [0, 255] and round to nearest integer
    int r_final = static_cast<int>(std::round(r_gamma * 255.0f));
    int g_final = static_cast<int>(std::round(g_gamma * 255.0f));
    int b_final = static_cast<int>(std::round(b_gamma * 255.0f));

    // Clamp the results to [0, 255]
    r_final = std::clamp(r_final, 0, 255);
    g_final = std::clamp(g_final, 0, 255);
    b_final = std::clamp(b_final, 0, 255);

    // Return the gamma-corrected Colour
    return Colour(r_final, g_final, b_final);
}

std::vector<int> computeHistogram(const std::vector<Colour>& pixels, int bins) {
    std::vector<int> histogram(bins, 0);

    for (const auto& pixel : pixels) {
        // Convert to grayscale using luminance formula
        float luminance = 0.2126f * pixel.r + 0.7152f * pixel.g + 0.0722f * pixel.b;
        int bin = std::clamp(static_cast<int>(luminance * (bins / 255.0f)), 0, bins - 1);
        histogram[bin]++;
    }

    return histogram;
}

std::vector<float> computeCDF(const std::vector<int>& histogram) {
    std::vector<float> cdf(histogram.size(), 0.0f);
    int totalPixels = std::accumulate(histogram.begin(), histogram.end(), 0);
    
    if (totalPixels == 0) return cdf;  // Prevent division by zero

    cdf[0] = static_cast<float>(histogram[0]) / totalPixels;
    for (size_t i = 1; i < histogram.size(); ++i) {
        cdf[i] = cdf[i - 1] + static_cast<float>(histogram[i]) / totalPixels;
    }

    return cdf;
}

Colour equalisePixel(const Colour& pixel, const std::vector<float>& cdf, float exposure) {
    // Convert pixel to luminance
    float luminance = 0.2126f * pixel.r + 0.7152f * pixel.g + 0.0722f * pixel.b;
    int bin = std::clamp(static_cast<int>(luminance * (cdf.size() / 255.0f)), 0, static_cast<int>(cdf.size() - 1));

    // Remap luminance using CDF and exposure
    float equalisedLuminance = cdf[bin] * 255.0f * exposure;

    // Scale original pixel by the equalised luminance
    float scale = (luminance > 0.0f) ? (equalisedLuminance / luminance) : 1.0f;

    Colour scaled = Colour(
        static_cast<int>(std::clamp(pixel.r * scale, 0.0f, 255.0f)),
        static_cast<int>(std::clamp(pixel.g * scale, 0.0f, 255.0f)),
        static_cast<int>(std::clamp(pixel.b * scale, 0.0f, 255.0f))
    );
    return scaled;
}

void applyHistogramEqualisation(std::vector<Colour>& pixels, float exposure) {
    auto histogram = computeHistogram(pixels, 256);  // Ensure 256 bins for full dynamic range
    auto cdf = computeCDF(histogram);

    for (auto& pixel : pixels) {
        pixel = equalisePixel(pixel, cdf, exposure);
    }
}