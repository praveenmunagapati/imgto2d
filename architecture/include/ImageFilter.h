#pragma once
#include <opencv2/core.hpp>
#include <string>

namespace DrawingBot {

    class ImageFilter {
    public:
        virtual ~ImageFilter() = default;
        virtual void apply(cv::Mat& image) = 0;
        virtual std::string getName() const = 0;
        bool enabled = true;
    };

    class DirtyBorderFilter : public ImageFilter { public: int width = 15; void apply(cv::Mat& img) override; std::string getName() const override { return "Dirty Border"; } };
    class CustomOverlayFilter : public ImageFilter { public: std::string overlayImagePath; float opacity = 0.5f; void apply(cv::Mat& img) override; std::string getName() const override { return "Custom Overlay"; } };

    class BoxBlurFilter : public ImageFilter { public: int radius = 3; void apply(cv::Mat& img) override; std::string getName() const override { return "Box Blur"; } };
    class EmbossEdgesFilter : public ImageFilter { public: void apply(cv::Mat& img) override; std::string getName() const override { return "Emboss Edges"; } };
    class GaussianBlurFilter : public ImageFilter { public: int radius = 3; void apply(cv::Mat& img) override; std::string getName() const override { return "Gaussian Blur"; } };
    class GlowFilter : public ImageFilter { public: int radius = 7; float amount = 0.5f; void apply(cv::Mat& img) override; std::string getName() const override { return "Glow"; } };
    class HighPassFilter : public ImageFilter { public: int radius = 3; void apply(cv::Mat& img) override; std::string getName() const override { return "High Pass"; } };
    class LensBlurFilter : public ImageFilter { public: int radius = 4; void apply(cv::Mat& img) override; std::string getName() const override { return "Lens Blur"; } };
    class MaximumFilter : public ImageFilter { public: int radius = 1; void apply(cv::Mat& img) override; std::string getName() const override { return "Maximum"; } };
    class MedianFilter : public ImageFilter { public: int radius = 2; void apply(cv::Mat& img) override; std::string getName() const override { return "Median"; } };
    class MinimumFilter : public ImageFilter { public: int radius = 1; void apply(cv::Mat& img) override; std::string getName() const override { return "Minimum"; } };
    class MotionBlurFastFilter : public ImageFilter { public: int length = 15; void apply(cv::Mat& img) override; std::string getName() const override { return "Motion Blur Fast"; } };
    class MotionBlurSlowFilter : public ImageFilter { public: int length = 25; void apply(cv::Mat& img) override; std::string getName() const override { return "Motion Blur Slow"; } };
    class SharpenFilter : public ImageFilter { public: int radius = 0; float amount = 1.5f; void apply(cv::Mat& img) override; std::string getName() const override { return "Sharpen"; } };
    class SimpleBlurFilter : public ImageFilter { public: int radius = 1; void apply(cv::Mat& img) override; std::string getName() const override { return "Simple Blur"; } };
    class SmartBlurFilter : public ImageFilter { public: int radius = 4; float sigma = 75.0f; void apply(cv::Mat& img) override; std::string getName() const override { return "Smart Blur"; } };
    class UnsharpMaskFilter : public ImageFilter { public: float radius = 2.0f; float amount = 1.5f; void apply(cv::Mat& img) override; std::string getName() const override { return "Unsharp Mask"; } };

    class AdjustHSBFilter : public ImageFilter { public: float hue = 0.0f; float sat = 0.0f; float bright = 0.0f; void apply(cv::Mat& img) override; std::string getName() const override { return "Adjust HSB"; } };
    class AdjustRGBFilter : public ImageFilter { public: float r = 10.0f; float g = 10.0f; float b = 10.0f; void apply(cv::Mat& img) override; std::string getName() const override { return "Adjust RGB"; } };
    class ContrastFilter : public ImageFilter { public: float contrast = 1.2f; void apply(cv::Mat& img) override; std::string getName() const override { return "Contrast"; } };
    class ExposureFilter : public ImageFilter { public: float exposure = 1.0f; float offset = 20.0f; void apply(cv::Mat& img) override; std::string getName() const override { return "Exposure"; } };
    class GainFilter : public ImageFilter { public: float gain = 1.1f; float bias = 10.0f; void apply(cv::Mat& img) override; std::string getName() const override { return "Gain"; } };
    class GammaFilter : public ImageFilter { public: float gamma = 1.2f; void apply(cv::Mat& img) override; std::string getName() const override { return "Gamma"; } };
    class GrayOutFilter : public ImageFilter { public: void apply(cv::Mat& img) override; std::string getName() const override { return "Gray Out"; } };
    class InvertFilter : public ImageFilter { public: void apply(cv::Mat& img) override; std::string getName() const override { return "Invert"; } };
    class LevelsFilter : public ImageFilter { public: int minLevel = 0; int maxLevel = 255; void apply(cv::Mat& img) override; std::string getName() const override { return "Levels"; } };
    class MixChannelsFilter : public ImageFilter { public: void apply(cv::Mat& img) override; std::string getName() const override { return "Mix Channels"; } };
    class PosterizeFilter : public ImageFilter { public: int levels = 8; void apply(cv::Mat& img) override; std::string getName() const override { return "Posterize"; } };
    class QuantizeFilter : public ImageFilter { public: int numColors = 4; void apply(cv::Mat& img) override; std::string getName() const override { return "Quantize"; } };
    class RescaleFilter : public ImageFilter { public: float scale = 0.5f; void apply(cv::Mat& img) override; std::string getName() const override { return "Rescale"; } };
    class SolarizeFilter : public ImageFilter { public: int threshold = 127; void apply(cv::Mat& img) override; std::string getName() const override { return "Solarize"; } };
    class TransparencyFilter : public ImageFilter { public: float opacity = 0.8f; void apply(cv::Mat& img) override; std::string getName() const override { return "Transparency"; } };

    class DiffuseFilter : public ImageFilter { public: float scale = 20.0f; void apply(cv::Mat& img) override; std::string getName() const override { return "Diffuse"; } };
    class DisplaceFilter : public ImageFilter { public: float amount = 1.0f; void apply(cv::Mat& img) override; std::string getName() const override { return "Displace"; } };
    class KaleidoscopeFilter : public ImageFilter { public: int sides = 4; void apply(cv::Mat& img) override; std::string getName() const override { return "Kaleidoscope"; } };
    class MarbleFilter : public ImageFilter { public: float turbulence = 50.0f; void apply(cv::Mat& img) override; std::string getName() const override { return "Marble"; } };
    class RippleFilter : public ImageFilter { public: float frequency = 5.0f; void apply(cv::Mat& img) override; std::string getName() const override { return "Ripple"; } };
    class ShearFilter : public ImageFilter { public: float shearX = 0.2f; float shearY = 0.2f; void apply(cv::Mat& img) override; std::string getName() const override { return "Shear"; } };
    class SwimFilter : public ImageFilter { public: float amount = 7.0f; void apply(cv::Mat& img) override; std::string getName() const override { return "Swim"; } };

    class DetectEdgesFilter : public ImageFilter { public: int threshold1 = 100; int threshold2 = 200; void apply(cv::Mat& img) override; std::string getName() const override { return "Detect Edges"; } };
    class LaplaceFilter : public ImageFilter { public: void apply(cv::Mat& img) override; std::string getName() const override { return "Laplace"; } };

    class ChromeFilter : public ImageFilter { public: float amount = 0.5f; void apply(cv::Mat& img) override; std::string getName() const override { return "Chrome"; } };
    class FeedbackFilter : public ImageFilter { public: float zoom = 0.9f; void apply(cv::Mat& img) override; std::string getName() const override { return "Feedback"; } };
    class GlintFilter : public ImageFilter { public: int length = 21; void apply(cv::Mat& img) override; std::string getName() const override { return "Glint"; } };
    class MirrorFilter : public ImageFilter { public: void apply(cv::Mat& img) override; std::string getName() const override { return "Mirror"; } };

    class ChromaKeyFilter : public ImageFilter { public: int tolerance = 100; void apply(cv::Mat& img) override; std::string getName() const override { return "Chroma Key"; } };

    class ColorHalftoneFilter : public ImageFilter { public: float radius = 5.0f; void apply(cv::Mat& img) override; std::string getName() const override { return "Color Halftone"; } };
    class CrystallizeFilter : public ImageFilter { public: float size = 10.0f; void apply(cv::Mat& img) override; std::string getName() const override { return "Crystallize"; } };
    class PointillizeFilter : public ImageFilter { public: float size = 5.0f; void apply(cv::Mat& img) override; std::string getName() const override { return "Pointillize"; } };

    class ScratchesFilter : public ImageFilter { public: int count = 1; void apply(cv::Mat& img) override; std::string getName() const override { return "Scratches"; } };

    class NoiseFilter : public ImageFilter { public: float amount = 10.0f; void apply(cv::Mat& img) override; std::string getName() const override { return "Noise"; } };
    class ContoursFilter : public ImageFilter { public: int levels = 50; void apply(cv::Mat& img) override; std::string getName() const override { return "Contours"; } };
    class DissolveFilter : public ImageFilter { public: int threshold = 240; void apply(cv::Mat& img) override; std::string getName() const override { return "Dissolve"; } };
    class DropShadowFilter : public ImageFilter { public: int offsetX = 10; int offsetY = 10; float opacity = 0.5f; void apply(cv::Mat& img) override; std::string getName() const override { return "Drop Shadow"; } };
    class EmbossFilter : public ImageFilter { public: void apply(cv::Mat& img) override; std::string getName() const override { return "Emboss"; } };
    class FlareFilter : public ImageFilter { public: int radius = 50; void apply(cv::Mat& img) override; std::string getName() const override { return "Flare"; } };
    class OilFilter : public ImageFilter { public: int radius = 10; void apply(cv::Mat& img) override; std::string getName() const override { return "Oil"; } };
    class RaysFilter : public ImageFilter { public: int length = 31; void apply(cv::Mat& img) override; std::string getName() const override { return "Rays"; } };
    class ShapeBurstFilter : public ImageFilter { public: void apply(cv::Mat& img) override; std::string getName() const override { return "Shape Burst"; } };
    class SparkleFilter : public ImageFilter { public: int radius = 5; void apply(cv::Mat& img) override; std::string getName() const override { return "Sparkle"; } };
    class StampFilter : public ImageFilter { public: int threshold = 127; void apply(cv::Mat& img) override; std::string getName() const override { return "Stamp"; } };
    class ThresholdFilter : public ImageFilter { public: int lowerThreshold = 127; int upperThreshold = 255; void apply(cv::Mat& img) override; std::string getName() const override { return "Threshold"; } };

} // namespace DrawingBot
