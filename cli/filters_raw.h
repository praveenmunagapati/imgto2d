#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>

// Forward declare SettingValue and PFMSetting to match the main ones
// --- base_filter.h ---
#pragma once
/**
 * base_filter.h — Base class for all image pre-processing filters.
 * Port of app/filters/base_filter.py
 */

#include <opencv2/core.hpp>

#include "core_types.h" // For PFMSetting and SettingType

class ImageFilter {
public:
    virtual ~ImageFilter() = default;

    virtual std::string name() const = 0;
    virtual std::string category() const { return "General"; }
    virtual std::string description() const { return ""; }

    SettingValue get(const std::string& key) const {
        auto it = m_settings.find(key);
        if (it != m_settings.end()) return it->second.currentValue();
        return SettingValue();
    }

    void set(const std::string& key, const SettingValue& value) {
        if (m_settings.count(key)) m_settings[key].value = value;
    }

    virtual cv::Mat process(const cv::Mat& image) = 0;

protected:
    virtual std::vector<PFMSetting> defineSettings() const { return {}; }
    
    void initSettings() {
        auto list = defineSettings();
        for (const auto& s : list) {
            m_settings[s.key] = s;
        }
    }
    std::map<std::string, PFMSetting> m_settings;
};

// ---------------------------------------------------------------------------
// Threshold Filter Implementation
// ---------------------------------------------------------------------------
class ThresholdFilter : public ImageFilter {
public:
    ThresholdFilter() { initSettings(); }
    std::string name() const override { return "Threshold"; }

protected:
    std::vector<PFMSetting> defineSettings() const override {
        return {
            { "threshold", "Threshold", SettingType::Integer, 127, SettingValue(), 0.0, 255.0, 0.0, 255.0, 1.0 }
        };
    }

public:
    cv::Mat process(const cv::Mat& image) override {
        int t = get("threshold").toInt();
        cv::Mat out;
        cv::threshold(image, out, t, 255, cv::THRESH_BINARY);
        return out;
    }
};




// --- basic_filters.h ---
#pragma once
/**
 * basic_filters.h — Basic Image Filters (Brightness, Contrast, Threshold, etc.)
 */

class BrightnessFilter : public ImageFilter {

public:
    explicit BrightnessFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Brightness"; }
    std::string category() const override { return "Color"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class ContrastFilter : public ImageFilter {

public:
    explicit ContrastFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Contrast"; }
    std::string category() const override { return "Color"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class InvertFilter : public ImageFilter {

public:
    explicit InvertFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Invert"; }
    std::string category() const override { return "Color"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};


class UnsharpMaskFilter : public ImageFilter {

public:
    explicit UnsharpMaskFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Unsharp Mask"; }
    std::string category() const override { return "Blur/Sharpen"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

// --- color_filters.h ---
#pragma once
/**
 * color_filters.h — Color and Adjustment Filters
 */

class GrayscaleFilter : public ImageFilter {

public:
    explicit GrayscaleFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Grayscale"; }
    std::string category() const override { return "Color"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class DesaturateFilter : public ImageFilter {

public:
    explicit DesaturateFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Desaturate"; }
    std::string category() const override { return "Color"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class SaturationFilter : public ImageFilter {

public:
    explicit SaturationFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Saturation"; }
    std::string category() const override { return "Color"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class HueFilter : public ImageFilter {

public:
    explicit HueFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Hue Shift"; }
    std::string category() const override { return "Color"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class GammaFilter : public ImageFilter {

public:
    explicit GammaFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Gamma Correction"; }
    std::string category() const override { return "Color"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class ExposureFilter : public ImageFilter {

public:
    explicit ExposureFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Exposure"; }
    std::string category() const override { return "Color"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class SepiaFilter : public ImageFilter {

public:
    explicit SepiaFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Sepia"; }
    std::string category() const override { return "Color"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class CLAHEFilter : public ImageFilter {

public:
    explicit CLAHEFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "CLAHE"; }
    std::string category() const override { return "Color"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class TemperatureFilter : public ImageFilter {

public:
    explicit TemperatureFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Temperature"; }
    std::string category() const override { return "Color"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

// --- edge_blur_filters.h ---
#pragma once
/**
 * edge_blur_filters.h — Edge Detection and Blur Filters
 */

// ---------------------------------------------------------------------------
// Edge Filters
// ---------------------------------------------------------------------------
class CannyFilter : public ImageFilter {

public:
    explicit CannyFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Canny Edge Detection"; }
    std::string category() const override { return "Edge Detection"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class SobelFilter : public ImageFilter {

public:
    explicit SobelFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Sobel Edge Detection"; }
    std::string category() const override { return "Edge Detection"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class LaplacianFilter : public ImageFilter {

public:
    explicit LaplacianFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Laplacian Edge Detection"; }
    std::string category() const override { return "Edge Detection"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class PrewittFilter : public ImageFilter {

public:
    explicit PrewittFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Prewitt Edge"; }
    std::string category() const override { return "Edge Detection"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class ScharrFilter : public ImageFilter {

public:
    explicit ScharrFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Scharr Edge"; }
    std::string category() const override { return "Edge Detection"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class DoGFilter : public ImageFilter {

public:
    explicit DoGFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Difference of Gaussians (DoG)"; }
    std::string category() const override { return "Edge Detection"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class RidgeDetectionFilter : public ImageFilter {

public:
    explicit RidgeDetectionFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Ridge Detection (Hessian)"; }
    std::string category() const override { return "Edge Detection"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class HighPassFilter : public ImageFilter {

public:
    explicit HighPassFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "High Pass"; }
    std::string category() const override { return "Edge Detection"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

// ---------------------------------------------------------------------------
// Blur Filters
// ---------------------------------------------------------------------------
class GaussianBlurFilter : public ImageFilter {

public:
    explicit GaussianBlurFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Gaussian Blur"; }
    std::string category() const override { return "Blur/Sharpen"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class MedianBlurFilter : public ImageFilter {

public:
    explicit MedianBlurFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Median Blur"; }
    std::string category() const override { return "Blur/Sharpen"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class BoxBlurFilter : public ImageFilter {

public:
    explicit BoxBlurFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Box Blur"; }
    std::string category() const override { return "Blur/Sharpen"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class MotionBlurFilter : public ImageFilter {

public:
    explicit MotionBlurFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Motion Blur"; }
    std::string category() const override { return "Blur/Sharpen"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class BilateralFilter : public ImageFilter {

public:
    explicit BilateralFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Bilateral Filter"; }
    std::string category() const override { return "Blur/Sharpen"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class LowPassFilter : public ImageFilter {

public:
    explicit LowPassFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Low Pass"; }
    std::string category() const override { return "Blur/Sharpen"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class SharpenMoreFilter : public ImageFilter {

public:
    explicit SharpenMoreFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Sharpen More"; }
    std::string category() const override { return "Blur/Sharpen"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

// --- morph_filters.h ---
#pragma once

// ---------------------------------------------------------------------------
// Morphological Filters
// ---------------------------------------------------------------------------
class DilationFilter : public ImageFilter {

public:
    explicit DilationFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Dilation"; }
    std::string category() const override { return "Morphological"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class ErosionFilter : public ImageFilter {

public:
    explicit ErosionFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Erosion"; }
    std::string category() const override { return "Morphological"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class OpeningFilter : public ImageFilter {

public:
    explicit OpeningFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Opening"; }
    std::string category() const override { return "Morphological"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class ClosingFilter : public ImageFilter {

public:
    explicit ClosingFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Closing"; }
    std::string category() const override { return "Morphological"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class MorphGradientFilter : public ImageFilter {

public:
    explicit MorphGradientFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Morphological Gradient"; }
    std::string category() const override { return "Morphological"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class TopHatFilter : public ImageFilter {

public:
    explicit TopHatFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Top Hat"; }
    std::string category() const override { return "Morphological"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class BlackHatFilter : public ImageFilter {

public:
    explicit BlackHatFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Black Hat"; }
    std::string category() const override { return "Morphological"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class DilateCrossFilter : public ImageFilter {

public:
    explicit DilateCrossFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Dilate Cross"; }
    std::string category() const override { return "Morphological"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class ErodeCrossFilter : public ImageFilter {

public:
    explicit ErodeCrossFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Erode Cross"; }
    std::string category() const override { return "Morphological"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class DilateEllipseFilter : public ImageFilter {

public:
    explicit DilateEllipseFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Dilate Ellipse"; }
    std::string category() const override { return "Morphological"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class ErodeEllipseFilter : public ImageFilter {

public:
    explicit ErodeEllipseFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Erode Ellipse"; }
    std::string category() const override { return "Morphological"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

// --- noise_filters.h ---
#pragma once
/**
 * noise_filters.h — Noise and Denoise Filters
 */
#include <random>

class GaussianNoiseFilter : public ImageFilter {

public:
    explicit GaussianNoiseFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Add Gaussian Noise"; }
    std::string category() const override { return "Noise"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class SaltAndPepperFilter : public ImageFilter {

public:
    explicit SaltAndPepperFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Salt & Pepper Noise"; }
    std::string category() const override { return "Noise"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class DenoiseFilter : public ImageFilter {

public:
    explicit DenoiseFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Denoise (NL Means)"; }
    std::string category() const override { return "Noise"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class GaussianNoise2Filter : public ImageFilter {

public:
    explicit GaussianNoise2Filter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Gaussian Noise 2"; }
    std::string category() const override { return "Noise"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class SpeckleNoiseFilter : public ImageFilter {

public:
    explicit SpeckleNoiseFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Speckle Noise"; }
    std::string category() const override { return "Noise"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};


// ---------------------------------------------------------------------------
// Threshold Filters
// ---------------------------------------------------------------------------
class OtsuThresholdFilter : public ImageFilter {

public:
    explicit OtsuThresholdFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Otsu Threshold"; }
    std::string category() const override { return "Threshold"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class AdaptiveThresholdFilter : public ImageFilter {

public:
    explicit AdaptiveThresholdFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Adaptive Threshold"; }
    std::string category() const override { return "Threshold"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class TruncateThresholdFilter : public ImageFilter {

public:
    explicit TruncateThresholdFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Truncate Threshold"; }
    std::string category() const override { return "Threshold"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class ToZeroThresholdFilter : public ImageFilter {

public:
    explicit ToZeroThresholdFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "To Zero Threshold"; }
    std::string category() const override { return "Threshold"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

// ---------------------------------------------------------------------------
// Extra/Color Filters
// ---------------------------------------------------------------------------
class EqualizeHistFilter : public ImageFilter {

public:
    explicit EqualizeHistFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Equalize Histogram"; }
    std::string category() const override { return "Color"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class AutoContrastFilter : public ImageFilter {

public:
    explicit AutoContrastFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Auto Contrast"; }
    std::string category() const override { return "Color"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class AutoColorFilter : public ImageFilter {

public:
    explicit AutoColorFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Auto Color"; }
    std::string category() const override { return "Color"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class ColorizeFilter : public ImageFilter {

public:
    explicit ColorizeFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Colorize (Tint)"; }
    std::string category() const override { return "Color"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class InvertHueFilter : public ImageFilter {

public:
    explicit InvertHueFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Invert Hue"; }
    std::string category() const override { return "Color"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

// --- artistic_distort_filters.h ---
#pragma once

// ---------------------------------------------------------------------------
// Artistic Filters
// ---------------------------------------------------------------------------
class PosterizeFilter : public ImageFilter {

public:
    explicit PosterizeFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Posterize"; }
    std::string category() const override { return "Artistic"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class EdgePreserveFilter : public ImageFilter {

public:
    explicit EdgePreserveFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Edge Preserve Smooth"; }
    std::string category() const override { return "Artistic"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class StylizationFilter : public ImageFilter {

public:
    explicit StylizationFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Stylization"; }
    std::string category() const override { return "Artistic"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class OilPaintingFilter : public ImageFilter {

public:
    explicit OilPaintingFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Oil Painting"; }
    std::string category() const override { return "Artistic"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class DetailEnhanceFilter : public ImageFilter {

public:
    explicit DetailEnhanceFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Detail Enhance"; }
    std::string category() const override { return "Artistic"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class PencilSketchFilter : public ImageFilter {

public:
    explicit PencilSketchFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Pencil Sketch"; }
    std::string category() const override { return "Artistic"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class EmbossFilter : public ImageFilter {

public:
    explicit EmbossFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Emboss"; }
    std::string category() const override { return "Stylize"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class QuantizeFilter : public ImageFilter {

public:
    explicit QuantizeFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Quantize (8 Colors)"; }
    std::string category() const override { return "Artistic"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

// ---------------------------------------------------------------------------
// Distort Filters
// ---------------------------------------------------------------------------
class VignetteFilter : public ImageFilter {

public:
    explicit VignetteFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Vignette"; }
    std::string category() const override { return "Distort"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class PixelateFilter : public ImageFilter {

public:
    explicit PixelateFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Pixelate"; }
    std::string category() const override { return "Distort"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class WaveFilter : public ImageFilter {

public:
    explicit WaveFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Wave Distortion"; }
    std::string category() const override { return "Distort"; }
public:
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

// ===========================================================================
// MISSING SPEC FILTERS — Borders
// ===========================================================================
class DirtyBorderFilter : public ImageFilter {
public:
    explicit DirtyBorderFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Dirty Border"; }
    std::string category() const override { return "Borders"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class CustomOverlayFilter : public ImageFilter {
public:
    explicit CustomOverlayFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Custom Overlay"; }
    std::string category() const override { return "Borders"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

// ===========================================================================
// MISSING SPEC FILTERS — Blur
// ===========================================================================
class GlowFilter : public ImageFilter {
public:
    explicit GlowFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Glow"; }
    std::string category() const override { return "Blur/Sharpen"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class LensBlurFilter : public ImageFilter {
public:
    explicit LensBlurFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Lens Blur"; }
    std::string category() const override { return "Blur/Sharpen"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class MaximumFilter : public ImageFilter {
public:
    explicit MaximumFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Maximum"; }
    std::string category() const override { return "Blur/Sharpen"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class MinimumFilter : public ImageFilter {
public:
    explicit MinimumFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Minimum"; }
    std::string category() const override { return "Blur/Sharpen"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class SmartBlurFilter : public ImageFilter {
public:
    explicit SmartBlurFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Smart Blur"; }
    std::string category() const override { return "Blur/Sharpen"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

// ===========================================================================
// MISSING SPEC FILTERS — Colors
// ===========================================================================
class AdjustHSBFilter : public ImageFilter {
public:
    explicit AdjustHSBFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Adjust HSB"; }
    std::string category() const override { return "Color"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class AdjustRGBFilter : public ImageFilter {
public:
    explicit AdjustRGBFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Adjust RGB"; }
    std::string category() const override { return "Color"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class GainFilter : public ImageFilter {
public:
    explicit GainFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Gain"; }
    std::string category() const override { return "Color"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class GrayOutFilter : public ImageFilter {
public:
    explicit GrayOutFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Gray Out"; }
    std::string category() const override { return "Color"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class LevelsFilter : public ImageFilter {
public:
    explicit LevelsFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Levels"; }
    std::string category() const override { return "Color"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class MixChannelsFilter : public ImageFilter {
public:
    explicit MixChannelsFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Mix Channels"; }
    std::string category() const override { return "Color"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class RescaleFilter : public ImageFilter {
public:
    explicit RescaleFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Rescale"; }
    std::string category() const override { return "Color"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class SolarizeFilter : public ImageFilter {
public:
    explicit SolarizeFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Solarize"; }
    std::string category() const override { return "Color"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class TransparencyFilter : public ImageFilter {
public:
    explicit TransparencyFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Transparency"; }
    std::string category() const override { return "Color"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

// ===========================================================================
// MISSING SPEC FILTERS — Distort
// ===========================================================================
class DiffuseFilter : public ImageFilter {
public:
    explicit DiffuseFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Diffuse"; }
    std::string category() const override { return "Distort"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class DisplaceFilter : public ImageFilter {
public:
    explicit DisplaceFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Displace"; }
    std::string category() const override { return "Distort"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class KaleidoscopeFilter : public ImageFilter {
public:
    explicit KaleidoscopeFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Kaleidoscope"; }
    std::string category() const override { return "Distort"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class MarbleFilter : public ImageFilter {
public:
    explicit MarbleFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Marble"; }
    std::string category() const override { return "Distort"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class RippleFilter : public ImageFilter {
public:
    explicit RippleFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Ripple"; }
    std::string category() const override { return "Distort"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class ShearFilter : public ImageFilter {
public:
    explicit ShearFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Shear"; }
    std::string category() const override { return "Distort"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class SwimFilter : public ImageFilter {
public:
    explicit SwimFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Swim"; }
    std::string category() const override { return "Distort"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

// ===========================================================================
// MISSING SPEC FILTERS — Effects
// ===========================================================================
class ChromeFilter : public ImageFilter {
public:
    explicit ChromeFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Chrome"; }
    std::string category() const override { return "Effects"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class FeedbackFilter : public ImageFilter {
public:
    explicit FeedbackFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Feedback"; }
    std::string category() const override { return "Effects"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class GlintFilter : public ImageFilter {
public:
    explicit GlintFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Glint"; }
    std::string category() const override { return "Effects"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class MirrorFilter : public ImageFilter {
public:
    explicit MirrorFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Mirror"; }
    std::string category() const override { return "Effects"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

// ===========================================================================
// MISSING SPEC FILTERS — Keying
// ===========================================================================
class ChromaKeyFilter : public ImageFilter {
public:
    explicit ChromaKeyFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Chroma Key"; }
    std::string category() const override { return "Keying"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

// ===========================================================================
// MISSING SPEC FILTERS — Pixellate
// ===========================================================================
class ColorHalftoneFilter : public ImageFilter {
public:
    explicit ColorHalftoneFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Color Halftone"; }
    std::string category() const override { return "Pixellate"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class CrystallizeFilter : public ImageFilter {
public:
    explicit CrystallizeFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Crystallize"; }
    std::string category() const override { return "Pixellate"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class PointillizeFilter : public ImageFilter {
public:
    explicit PointillizeFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Pointillize"; }
    std::string category() const override { return "Pixellate"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

// ===========================================================================
// MISSING SPEC FILTERS — Render
// ===========================================================================
class ScratchesFilter : public ImageFilter {
public:
    explicit ScratchesFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Scratches"; }
    std::string category() const override { return "Render"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

// ===========================================================================
// MISSING SPEC FILTERS — Stylize
// ===========================================================================
class ContoursFilter : public ImageFilter {
public:
    explicit ContoursFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Contours"; }
    std::string category() const override { return "Stylize"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class DissolveFilter : public ImageFilter {
public:
    explicit DissolveFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Dissolve"; }
    std::string category() const override { return "Stylize"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class DropShadowFilter : public ImageFilter {
public:
    explicit DropShadowFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Drop Shadow"; }
    std::string category() const override { return "Stylize"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class FlareFilter : public ImageFilter {
public:
    explicit FlareFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Flare"; }
    std::string category() const override { return "Stylize"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class OilFilter : public ImageFilter {
public:
    explicit OilFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Oil"; }
    std::string category() const override { return "Stylize"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class RaysFilter : public ImageFilter {
public:
    explicit RaysFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Rays"; }
    std::string category() const override { return "Stylize"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class ShapeBurstFilter : public ImageFilter {
public:
    explicit ShapeBurstFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Shape Burst"; }
    std::string category() const override { return "Stylize"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class SparkleFilter : public ImageFilter {
public:
    explicit SparkleFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Sparkle"; }
    std::string category() const override { return "Stylize"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class StampFilter : public ImageFilter {
public:
    explicit StampFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Stamp"; }
    std::string category() const override { return "Stylize"; }
    std::vector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};


std::unique_ptr<ImageFilter> create_filter(const std::string& name);


