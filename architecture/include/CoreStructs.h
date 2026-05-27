#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace DrawingBot {

    struct DrawingArea {
        bool useOriginalSizing;
        enum class InputUnits { MM, CM, INCHES, PIXELS } inputUnits;
        float width;
        float height;
        enum class Orientation { PORTRAIT, LANDSCAPE } orientation;
        float paddingLeft, paddingRight, paddingTop, paddingBottom;
        enum class ScalingMode { CROP_TO_FIT, SCALE_TO_FIT, STRETCH_TO_FIT } scalingMode;
        bool rescaleToPenWidth;
        enum class RescaleMode { HIGH_QUALITY, LOW_QUALITY, OFF } rescaleMode;
        float penWidth; // in mm
        enum class ClippingMode { DRAWING, PAGE, NONE } clippingMode;
        uint32_t canvasColour;     // RGBA
        uint32_t backgroundColour; // RGBA
    };

    struct MaskSettings {
        bool enableMasking;
        bool softClip; // true=algorithmic avoidance, false=hard slice
    };

    struct Mask {
        bool enabled;
        std::string name;
        enum class MaskType { ADD, SUBTRACT } maskType;
        enum class Shape { RECTANGLE, CIRCLE, STAR, X_SHAPE, SVG_PATH } shape;
        float startX;
        float startY;
        float width;
        float height;
        float rotation; // 0.0 to 360.0
        float skewX;    // -180.0 to 180.0
        float skewY;    // -180.0 to 180.0
        std::string svgPath; // only used when shape == SVG_PATH
    };

    struct DrawingPen {
        bool enabled;
        std::string type;  // Manufacturer name or "Special"
        std::string name;
        uint32_t color;    // RGBA
        float weight;      // proportion of shapes assigned
        float stroke;      // rendered line thickness
    };

    struct PenSettings {
        std::vector<DrawingPen> pens;
        
        enum class DistributionType {
            EVEN_WEIGHTED,
            RANDOM_WEIGHTED,
            RANDOM_SQUIGGLES,
            LUMINANCE_WEIGHTED,
            PRECONFIGURED,
            SINGLE_PEN
        } distributionType;
        
        enum class DistributionOrder {
            DARKEST_FIRST,
            LIGHTEST_FIRST,
            DISPLAYED,
            REVERSED
        } distributionOrder;
        
        enum class ColorSeparation {
            DEFAULT,
            CMYK,
            CMYK_EXTENDED, // adds Orange, Green, Violet, Red, Blue
            COLOUR_MATCH
        } colorSeparation;
        
        // CMYK Settings
        float cmykMultiplierC;
        float cmykMultiplierM;
        float cmykMultiplierY;
        float cmykMultiplierK;
        float cmykCleanBlack;   // 0.0 to 1.0
        float cmykCleanWhite;   // 0.0 to 1.0
        float cmykVibrancy;     // extended gamut intensity
        float cmykReduceOverlay;
        
        // Colour Match Settings
        int   colourAccuracy;        // 95 to 100 (Delta-E threshold)
        float brightnessMultiplier;  // 0.0 to N
        int   penLimit;              // 0 = unlimited
        bool  useCanvasColour;
        bool  smoothDistribution;
        bool  invertColour;
    };

    struct PathOptimizationSettings {
        bool enableSimplifying; // Douglas-Peucker algorithm
        bool enableMerging;     // connects path endpoints within tolerance
        bool enableFiltering;   // deletes paths shorter than threshold
        bool enableSorting;     // STRTree nearest-neighbor sort
        int  multipass;         // 1=standard, N=repeat geometry N times
        
        float simplifyTolerance; // max deviation for Douglas-Peucker
        float mergeTolerance;    // max distance for endpoint connection
        float filterMinLength;   // minimum path length to keep
    };

    struct BatchSettings {
        std::string inputFolder;
        std::string outputFolder;
        bool        overwriteExisting;
    };

    struct GCodeSettings {
        float offsetX = 0.0f;
        float offsetY = 0.0f;
        float curveFlatness = 0.1f;
        bool centerZeroPoint = false;
        int commentType = 0;
        std::string gcodeStart;
        std::string gcodeEnd;
        std::string gcodePenDown;
        std::string gcodePenUp;
        std::string gcodeStartLayer;
        std::string gcodeEndLayer;
    };

    struct HPGLSettings {
        float hardClipMinX = 0.0f;
        float hardClipMinY = 0.0f;
        float hardClipMaxX = 10000.0f;
        float hardClipMaxY = 10000.0f;
        float rotation = 0.0f;
        bool xAxisMirror = false;
        bool yAxisMirror = false;
        int penVelocity = 10;
        int penForce = 1;
        int initialPen = 1;
    };

} // namespace DrawingBot
