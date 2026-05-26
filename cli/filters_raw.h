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
public:
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
public:
    cv::Mat process(const cv::Mat& image) override;
};

class DesaturateFilter : public ImageFilter {

public:
    explicit DesaturateFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Desaturate"; }
    std::string category() const override { return "Color"; }
public:
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
public:
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
public:
    cv::Mat process(const cv::Mat& image) override;
};

class ScharrFilter : public ImageFilter {

public:
    explicit ScharrFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Scharr Edge"; }
    std::string category() const override { return "Edge Detection"; }
public:
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
    cv::Mat process(const cv::Mat& image) override;
};

class HighPassFilter : public ImageFilter {

public:
    explicit HighPassFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "High Pass"; }
    std::string category() const override { return "Edge Detection"; }
public:
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
public:
    cv::Mat process(const cv::Mat& image) override;
};

class LowPassFilter : public ImageFilter {

public:
    explicit LowPassFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Low Pass"; }
    std::string category() const override { return "Blur/Sharpen"; }
public:
    cv::Mat process(const cv::Mat& image) override;
};

class SharpenMoreFilter : public ImageFilter {

public:
    explicit SharpenMoreFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Sharpen More"; }
    std::string category() const override { return "Blur/Sharpen"; }
public:
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
public:
    cv::Mat process(const cv::Mat& image) override;
};

class ErodeCrossFilter : public ImageFilter {

public:
    explicit ErodeCrossFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Erode Cross"; }
    std::string category() const override { return "Morphological"; }
public:
    cv::Mat process(const cv::Mat& image) override;
};

class DilateEllipseFilter : public ImageFilter {

public:
    explicit DilateEllipseFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Dilate Ellipse"; }
    std::string category() const override { return "Morphological"; }
public:
    cv::Mat process(const cv::Mat& image) override;
};

class ErodeEllipseFilter : public ImageFilter {

public:
    explicit ErodeEllipseFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Erode Ellipse"; }
    std::string category() const override { return "Morphological"; }
public:
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
public:
    cv::Mat process(const cv::Mat& image) override;
};

class SpeckleNoiseFilter : public ImageFilter {

public:
    explicit SpeckleNoiseFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Speckle Noise"; }
    std::string category() const override { return "Noise"; }
public:
    cv::Mat process(const cv::Mat& image) override;
};


// --- threshold_extra_filters.h ---
#pragma once

// ---------------------------------------------------------------------------
// Threshold Filters
// ---------------------------------------------------------------------------
class OtsuThresholdFilter : public ImageFilter {

public:
    explicit OtsuThresholdFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Otsu Threshold"; }
    std::string category() const override { return "Threshold"; }
public:
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
public:
    cv::Mat process(const cv::Mat& image) override;
};

class AutoContrastFilter : public ImageFilter {

public:
    explicit AutoContrastFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Auto Contrast"; }
    std::string category() const override { return "Color"; }
public:
    cv::Mat process(const cv::Mat& image) override;
};

class AutoColorFilter : public ImageFilter {

public:
    explicit AutoColorFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Auto Color"; }
    std::string category() const override { return "Color"; }
public:
    cv::Mat process(const cv::Mat& image) override;
};

class ColorizeFilter : public ImageFilter {

public:
    explicit ColorizeFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Colorize (Tint)"; }
    std::string category() const override { return "Color"; }
public:
    cv::Mat process(const cv::Mat& image) override;
};

class InvertHueFilter : public ImageFilter {

public:
    explicit InvertHueFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Invert Hue"; }
    std::string category() const override { return "Color"; }
public:
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
public:
    cv::Mat process(const cv::Mat& image) override;
};

class StylizationFilter : public ImageFilter {

public:
    explicit StylizationFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Stylization"; }
    std::string category() const override { return "Artistic"; }
public:
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
public:
    cv::Mat process(const cv::Mat& image) override;
};

class QuantizeFilter : public ImageFilter {

public:
    explicit QuantizeFilter() : ImageFilter() { initSettings(); }
    std::string name() const override { return "Quantize (8 Colors)"; }
    std::string category() const override { return "Artistic"; }
public:
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

