#pragma once
#include <string>

namespace DrawingBot {

    class CircularScribblesBase {
    public:
        float minRadius;         // 0.1-128.0
        float maxRadius;         // 0.1-128.0
        float minVelocity;       // 0.1-128.0
        float maxVelocity;       // 0.1-128.0
        float angularVelocity;   // 0.1-180.0
        float azimuthAngleMin;   // -180-180
        float azimuthAngleMax;   // -180-180
        float polarAngleMin;     // -180-180
        float polarAngleMax;     // -180-180
        float curvature;         // 0-1
        bool  edgeRetention;
        float edgeThresholdA;    // 1.0-255.0
        float edgeThresholdB;    // 1.0-255.0
    };

    class ShapesBase {
    public:
        enum class ShapeType {
            CIRCLE, SQUARE, STAR, TRIANGLE, CROSS,
            MULTIPLY, LP_SPACE, RANDOM
        } shapeType;
        bool  alignRotation;
        float minRotation;       // 0-360
        float maxRotation;       // 0-360
        float fillSize;          // formula: finalRadius = sampleRadius * (fillSize/100)
    };

    struct LetterParams {
        enum class LetterOrder { TONE_MAPPED, RANDOM, SEQUENCED } order;
        std::string characterFilter;
        std::string regExFilter;
        bool        useSVGFonts;
        std::string svgFont;
        std::string font;
        enum class FontStyle { PLAIN, BOLD, ITALIC } style;
    };

} // namespace DrawingBot
