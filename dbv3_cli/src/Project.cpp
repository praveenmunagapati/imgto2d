
#include "../include/Project.h"
#include "../include/ImageFilter.h"
#include "../include/PFMSettings.h"
#include "../include/CoreStructs.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

namespace DrawingBot {

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DrawingArea, useOriginalSizing, inputUnits, width, height, orientation, paddingLeft, paddingRight, paddingTop, paddingBottom, scalingMode, rescaleToPenWidth, rescaleMode, penWidth, clippingMode, canvasColour, backgroundColour)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MaskSettings, enableMasking, softClip)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Mask, enabled, name, maskType, shape, startX, startY, width, height, rotation, skewX, skewY, svgPath)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DrawingPen, enabled, type, name, color, weight, stroke)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PenSettings, pens, distributionType, distributionOrder, colorSeparation, cmykMultiplierC, cmykMultiplierM, cmykMultiplierY, cmykMultiplierK, cmykCleanBlack, cmykCleanWhite, cmykVibrancy, cmykReduceOverlay, colourAccuracy, brightnessMultiplier, penLimit, useCanvasColour, smoothDistribution, invertColour)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PathOptimizationSettings, enableSimplifying, enableMerging, enableFiltering, enableSorting, multipass, simplifyTolerance, mergeTolerance, filterMinLength)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GCodeSettings, offsetX, offsetY, curveFlatness, centerZeroPoint, commentType, gcodeStart, gcodeEnd, gcodePenDown, gcodePenUp, gcodeStartLayer, gcodeEndLayer)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HPGLSettings, hardClipMinX, hardClipMinY, hardClipMaxX, hardClipMaxY, rotation, xAxisMirror, yAxisMirror, penVelocity, penForce, initialPen)
    
    // PFMSettings are now parsed manually by ProjectParser so we don't define macro here
    // ImageFilters
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DirtyBorderFilter, enabled, width)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CustomOverlayFilter, enabled, overlayImagePath, opacity)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BoxBlurFilter, enabled, radius)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(EmbossEdgesFilter, enabled)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GaussianBlurFilter, enabled, radius)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GlowFilter, enabled, radius, amount)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HighPassFilter, enabled, radius)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LensBlurFilter, enabled, radius)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MaximumFilter, enabled, radius)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MedianFilter, enabled, radius)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MinimumFilter, enabled, radius)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MotionBlurFastFilter, enabled, length)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MotionBlurSlowFilter, enabled, length)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SharpenFilter, enabled, radius, amount)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SimpleBlurFilter, enabled, radius)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SmartBlurFilter, enabled, radius, sigma)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UnsharpMaskFilter, enabled, radius, amount)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AdjustHSBFilter, enabled, hue, sat, bright)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AdjustRGBFilter, enabled, r, g, b)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ContrastFilter, enabled, contrast)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ExposureFilter, enabled, exposure, offset)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GainFilter, enabled, gain, bias)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GammaFilter, enabled, gamma)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GrayOutFilter, enabled)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InvertFilter, enabled)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LevelsFilter, enabled, minLevel, maxLevel)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MixChannelsFilter, enabled)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PosterizeFilter, enabled, levels)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(QuantizeFilter, enabled, numColors)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RescaleFilter, enabled, scale)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SolarizeFilter, enabled, threshold)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TransparencyFilter, enabled, opacity)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DiffuseFilter, enabled, scale)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DisplaceFilter, enabled, amount)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(KaleidoscopeFilter, enabled, sides)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MarbleFilter, enabled, turbulence)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RippleFilter, enabled, frequency)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ShearFilter, enabled, shearX, shearY)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SwimFilter, enabled, amount)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DetectEdgesFilter, enabled, threshold1, threshold2)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LaplaceFilter, enabled)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ChromeFilter, enabled, amount)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FeedbackFilter, enabled, zoom)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GlintFilter, enabled, length)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MirrorFilter, enabled)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ChromaKeyFilter, enabled, tolerance)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ColorHalftoneFilter, enabled, radius)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CrystallizeFilter, enabled, size)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PointillizeFilter, enabled, size)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ScratchesFilter, enabled, count)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(NoiseFilter, enabled, amount)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ContoursFilter, enabled, levels)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DissolveFilter, enabled, threshold)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DropShadowFilter, enabled, offsetX, offsetY, opacity)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(EmbossFilter, enabled)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FlareFilter, enabled, radius)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(OilFilter, enabled, radius)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RaysFilter, enabled, length)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ShapeBurstFilter, enabled)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SparkleFilter, enabled, radius)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(StampFilter, enabled, threshold)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ThresholdFilter, enabled, lowerThreshold, upperThreshold)
    
    // Note: ProjectVersion has a pointer to PFMBase, so we skip it for basic non-intrusive macro 
    // and manually serialize ProjectVersion if needed, or just exclude pfmConfig for the top-level dump right now.
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ProjectVersion, name, rating, notes, sourceImagePath, drawingArea, penSettings, masks)
    
    // Project serialization
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Project, name, sourceImagePath, drawingArea, maskSettings, penSettings, optimization, gcodeSettings, hpglSettings, versions)

    bool Project::saveToFile(const std::string& filepath) const {
        std::cout << "Saving massively parameterized DBV3 project to " << filepath << "...\\n";
        try {
            json j = *this;
            std::ofstream out(filepath);
            if(!out) return false;
            out << j.dump(4);
            out.close();
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Serialization error: " << e.what() << "\n";
            return false;
        }
    }

    bool Project::loadFromFile(const std::string& filepath) {
        std::cout << "Loading massively parameterized DBV3 project from " << filepath << "...\\n";
        try {
            std::ifstream in(filepath);
            if(!in) return false;
            json j;
            in >> j;
            *this = j.get<Project>();
            in.close();
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Deserialization error: " << e.what() << "\n";
            return false;
        }
    }
}
