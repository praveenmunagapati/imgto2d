#pragma once
#include "PFMBase.h"
#include "PFMSettings.h"

namespace DrawingBot {

    class SketchBase : public PFMBase {
    public:
        SketchSettings settings;
        // Style
        bool  shouldLiftPen;
        float directionality;       // 0–100
        float clarity;              // 0–100
        float distortion;           // 0–100
        float angularity;           // 0–100
        float edgePower;            // 0–100 (Canny edge detection weight)
        float sobelPower;           // 0–100 (Sobel edge filter weight)
        float luminancePower;       // 0–100
        float drawingDeltaAngle;    // -360 to 360

        // Segments
        float lineDensity;          // 0–100 (percentage, drives stop condition)
        int   lineMinLength;        // 2–500
        int   lineMaxLength;        // 2–500
        int   lineMaxLimit;         // -1 to 1000000 (-1 = ignore)
        int   angleTests;           // 1–360
        bool  unlimitedTests;

        // Squiggles
        int   squiggleMinLength;    // 0–5000
        int   squiggleMaxLength;    // 0–5000
        float squiggleMaxDeviation; // 0–100 percentage

        // Erasing
        int   eraseMin;             // 0–255
        int   eraseMax;             // 0–255
        float eraseRadiusMin;       // 0–50
        float eraseRadiusMax;       // 0–50
        float tone;                 // 0–100 percentage

        // Shading
        bool  shading;
        float startAngleMin;        // -360 to 360
        float startAngleMax;        // -360 to 360
        float shadingThreshold;     // 0–100 percentage
        float shadingDeltaAngle;    // -360 to 360
    };

    class SketchLines : public SketchBase { public: std::vector<PlotPath> generate(const cv::Mat& ref) override; };

    class SketchCurves : public SketchBase {
    public:
        float curveTension; // 0.01-1.0
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class SketchSquares : public SketchBase {
    public:
        float startAngle; // -360 to 360
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class SketchQuadBeziers : public SketchBase {
    public:
        int   curveTests;     // 1-45
        float curveVariation; // 1-1000
        float curveOffset;    // -100 to 100
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class SketchCubicBeziers : public SketchBase {
    public:
        int   curveTests;      // 1-45
        float curveVariation;  // 1-1000
        float curveOffsetA;    // -100 to 100
        float curveOffsetB;    // -100 to 100
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class SketchCatmullRoms : public SketchBase {
    public:
        float curveTension; // 0.01-1.0
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class SketchShapes : public SketchBase {
    public:
        enum class ShapeType { RECTANGLES, ELLIPSES } shapeType;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class SketchSobelEdges : public SketchBase {
    public:
        float sobelIntensity; // 0-10
        int   sobelAdjust;    // 0-255
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class SketchWaves : public SketchBase {
    public:
        float startAngle;     // -360 to 360
        float waveOffsetX;    // -1000 to 100
        float waveOffsetY;    // -1000 to 100
        float waveDivisorX;   // -1000 to 1000
        float waveDivisorY;   // -1000 to 1000
        enum class WaveType { SIN, COS, TAN };
        WaveType waveTypeX;
        WaveType waveTypeY;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class SketchFlowField : public SketchBase {
    public:
        float startAngle;      // -360 to 360
        float xFrequency;      // 0.001-4.0
        float yFrequency;      // 0.001-4.0
        float scaleFrequency;  // 0.01-20.0
        float amplitude;       // 0.0-1.0
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class SketchSuperformula : public SketchBase {
    public:
        float startAngle;  // -360 to 360
        float centreX;     // 0.0-100.0
        float centreY;     // 0.0-100.0
        float xScale;      // 1.0-20.0
        float yScale;      // 1.0-20.0
        float frequency;   // 0.0-20.0
        float curvature;   // 0.0-80.0
        float sineFactor;  // 0.0-80.0
        float cosFactor;   // 0.0-80.0
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class SketchSweepingCurves : public SketchBase {
    public:
        float curvature; // 0.0-1.0
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

} // namespace DrawingBot
