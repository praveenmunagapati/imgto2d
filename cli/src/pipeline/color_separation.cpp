#include "color_separation.h"
#include <opencv2/imgproc.hpp>
#include <algorithm>
#include <cmath>
#include <string>
#include <stdexcept>

std::vector<cv::Mat> splitGrayscale(const cv::Mat& bgrImage) {
    if (bgrImage.channels() == 1) {
        return {bgrImage.clone()};
    }
    cv::Mat gray;
    if (bgrImage.channels() == 4) {
        cv::cvtColor(bgrImage, gray, cv::COLOR_BGRA2GRAY);
    } else {
        cv::cvtColor(bgrImage, gray, cv::COLOR_BGR2GRAY);
    }
    return {gray};
}

std::vector<cv::Mat> splitRGB(const cv::Mat& bgrImage) {
    if (bgrImage.channels() == 1) {
        return {bgrImage.clone(), bgrImage.clone(), bgrImage.clone()};
    }
    
    cv::Mat bgr;
    if (bgrImage.channels() == 4) {
        cv::cvtColor(bgrImage, bgr, cv::COLOR_BGRA2BGR);
    } else {
        bgr = bgrImage;
    }
    
    std::vector<cv::Mat> channels;
    cv::split(bgr, channels);
    
    cv::Mat rDensity, gDensity, bDensity;
    cv::bitwise_not(channels[2], rDensity);
    cv::bitwise_not(channels[1], gDensity);
    cv::bitwise_not(channels[0], bDensity);
    
    return {rDensity, gDensity, bDensity};
}

std::vector<cv::Mat> splitCMYK(const cv::Mat& bgrImage) {
    if (bgrImage.channels() == 1) {
        return {bgrImage.clone(), bgrImage.clone(), bgrImage.clone(), bgrImage.clone()};
    }
    
    cv::Mat bgr;
    if (bgrImage.channels() == 4) {
        cv::cvtColor(bgrImage, bgr, cv::COLOR_BGRA2BGR);
    } else {
        bgr = bgrImage;
    }
    
    int rows = bgr.rows;
    int cols = bgr.cols;
    
    cv::Mat cDensity(rows, cols, CV_8UC1);
    cv::Mat mDensity(rows, cols, CV_8UC1);
    cv::Mat yDensity(rows, cols, CV_8UC1);
    cv::Mat kDensity(rows, cols, CV_8UC1);
    
    for (int r = 0; r < rows; ++r) {
        const uchar* ptr = bgr.ptr<uchar>(r);
        uchar* cPtr = cDensity.ptr<uchar>(r);
        uchar* mPtr = mDensity.ptr<uchar>(r);
        uchar* yPtr = yDensity.ptr<uchar>(r);
        uchar* kPtr = kDensity.ptr<uchar>(r);
        
        for (int c = 0; c < cols; ++c) {
            float blue = ptr[c * 3 + 0] / 255.0f;
            float green = ptr[c * 3 + 1] / 255.0f;
            float red = ptr[c * 3 + 2] / 255.0f;
            
            float k = 1.0f - std::max({red, green, blue});
            float cyan = 0.0f;
            float magenta = 0.0f;
            float yellow = 0.0f;
            
            if (k < 1.0f) {
                cyan = (1.0f - red - k) / (1.0f - k);
                magenta = (1.0f - green - k) / (1.0f - k);
                yellow = (1.0f - blue - k) / (1.0f - k);
            }
            
            cPtr[c] = static_cast<uchar>(cyan * 255.0f);
            mPtr[c] = static_cast<uchar>(magenta * 255.0f);
            yPtr[c] = static_cast<uchar>(yellow * 255.0f);
            kPtr[c] = static_cast<uchar>(k * 255.0f);
        }
    }
    
    return {cDensity, mDensity, yDensity, kDensity};
}

// ---------------------------------------------------------------------------
// Delta-E Colour Match separation — CIELAB / CIE76
// ---------------------------------------------------------------------------
namespace {

/**
 * Convert an sRGB triplet [0,255] to CIELAB.
 * Uses the standard D65 illuminant and the CIE 1931 2° observer.
 */
struct LabColor { float L, a, b; };

static float linearize(float c) {
    c /= 255.0f;
    return (c <= 0.04045f) ? (c / 12.92f) : std::pow((c + 0.055f) / 1.055f, 2.4f);
}

static float f_lab(float t) {
    constexpr float delta = 6.0f / 29.0f;
    return (t > delta * delta * delta)
        ? std::cbrt(t)
        : (t / (3.0f * delta * delta) + 4.0f / 29.0f);
}

static LabColor bgr_to_lab(uchar bv, uchar gv, uchar rv) {
    // sRGB → linear RGB
    float r = linearize(static_cast<float>(rv));
    float g = linearize(static_cast<float>(gv));
    float b = linearize(static_cast<float>(bv));

    // Linear RGB → XYZ (D65)
    float X = r * 0.4124564f + g * 0.3575761f + b * 0.1804375f;
    float Y = r * 0.2126729f + g * 0.7151522f + b * 0.0721750f;
    float Z = r * 0.0193339f + g * 0.1191920f + b * 0.9503041f;

    // Normalize by D65 white point
    X /= 0.95047f;
    // Y / 1.00000 = Y
    Z /= 1.08883f;

    float fx = f_lab(X);
    float fy = f_lab(Y);
    float fz = f_lab(Z);

    LabColor lab;
    lab.L = 116.0f * fy - 16.0f;
    lab.a = 500.0f * (fx - fy);
    lab.b = 200.0f * (fy - fz);
    return lab;
}

static float delta_e76(const LabColor& a, const LabColor& b) {
    float dL = a.L - b.L;
    float da = a.a - b.a;
    float db = a.b - b.b;
    return std::sqrt(dL * dL + da * da + db * db);
}

/** Parse "#RRGGBB" or "RRGGBB" hex string to BGR triplet. */
static cv::Vec3b hex_to_bgr(const std::string& hex) {
    std::string h = hex;
    if (!h.empty() && h[0] == '#') h = h.substr(1);
    if (h.size() < 6) return {128, 128, 128};
    unsigned int r = std::stoul(h.substr(0, 2), nullptr, 16);
    unsigned int g = std::stoul(h.substr(2, 2), nullptr, 16);
    unsigned int bv = std::stoul(h.substr(4, 2), nullptr, 16);
    return {static_cast<uchar>(bv), static_cast<uchar>(g), static_cast<uchar>(r)};
}

} // anonymous namespace

std::vector<cv::Mat> splitColourMatch(const cv::Mat& bgrImage,
                                      const std::vector<std::string>& penColorsHex,
                                      double colourAccuracy,
                                      double brightnessMult)
{
    const int nPens = static_cast<int>(penColorsHex.size());
    if (nPens == 0) return splitGrayscale(bgrImage);

    // Pre-convert pen colours to Lab
    std::vector<LabColor> penLab(nPens);
    for (int i = 0; i < nPens; ++i) {
        auto bgr = hex_to_bgr(penColorsHex[i]);
        penLab[i] = bgr_to_lab(bgr[0], bgr[1], bgr[2]);
    }

    // Delta-E threshold: accuracy=100 → threshold=0 (exact), accuracy=0 → threshold=100 (accept all)
    const float deThreshold = static_cast<float>(100.0 - colourAccuracy);
    const float bMult = static_cast<float>(std::max(0.0, std::min(4.0, brightnessMult)));

    // Ensure input is BGR
    cv::Mat bgr;
    if (bgrImage.channels() == 1) {
        cv::cvtColor(bgrImage, bgr, cv::COLOR_GRAY2BGR);
    } else if (bgrImage.channels() == 4) {
        cv::cvtColor(bgrImage, bgr, cv::COLOR_BGRA2BGR);
    } else {
        bgr = bgrImage;
    }

    int rows = bgr.rows;
    int cols = bgr.cols;

    // One density map per pen
    std::vector<cv::Mat> densities(nPens, cv::Mat::zeros(rows, cols, CV_8UC1));

    for (int r = 0; r < rows; ++r) {
        const uchar* src = bgr.ptr<uchar>(r);
        for (int c = 0; c < cols; ++c) {
            uchar bv = src[c * 3 + 0];
            uchar gv = src[c * 3 + 1];
            uchar rv = src[c * 3 + 2];

            // Pixel brightness (luminance proxy) — used as ink density
            float brightness = (0.299f * rv + 0.587f * gv + 0.114f * bv);
            float density = std::min(255.0f, brightness * bMult);

            // Skip near-white pixels (very low ink density)
            if (density < 2.0f) continue;

            LabColor pixLab = bgr_to_lab(bv, gv, rv);

            // Find closest pen colour within threshold
            float bestDE = std::numeric_limits<float>::max();
            int   bestPen = -1;
            for (int p = 0; p < nPens; ++p) {
                float de = delta_e76(pixLab, penLab[p]);
                if (de < bestDE) {
                    bestDE = de;
                    bestPen = p;
                }
            }

            if (bestPen >= 0 && bestDE <= deThreshold) {
                // Ink density = how different the pixel is from white (inverted luminance)
                uchar inkDensity = static_cast<uchar>(255.0f - density);
                // Clamp to 0-255
                densities[bestPen].ptr<uchar>(r)[c] = inkDensity;
            }
        }
    }

    return densities;
}
