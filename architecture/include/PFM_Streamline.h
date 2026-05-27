#pragma once
#include "PFMBase.h"
#include "PFMSettings.h"

namespace DrawingBot {

    class StreamlineBase : public PFMBase {
    public:
        StreamlineSettings settings;
        float minSpacing;   // 0.5-20.0
        float maxSpacing;   // 2.0-50.0
        float minLength;    // 0.0-20.0
        float maxLength;    // 0.0-500.0
        float tone;         // 0.0-100.0
        float distortion;   // 0.0-100.0
    };

    class StreamlinesEdgeField : public StreamlineBase {
    public:
        // Flow field params
        float startAngle;      // -360 to 360
        float xFrequency;      // 0.001-4.0
        float yFrequency;      // 0.001-4.0
        float scaleFrequency;  // 0.01-20.0
        float amplitude;       // 0.0-1.0
        
        // Edge field specifics
        float edgePower;           // 0.0-100.0
        int   etfIterations;       // 0-100
        float etfRadius;           // 3.0-30.0
        int   postBlurIterations;  // 0-50
        float postBlurRadius;      // 0.0-50.0
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class StreamlinesFlowField : public StreamlineBase {
    public:
        float startAngle;     // -360 to 360
        float xFrequency;     // 0.001-4.0
        float yFrequency;     // 0.001-4.0
        float scaleFrequency; // 0.01-20.0
        float amplitude;      // 0.0-1.0
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class StreamlinesSuperformula : public StreamlineBase {
    public:
        float startAngle;
        float centreX;
        float centreY;
        float xScale;
        float yScale;
        float frequency;
        float curvature;
        float sineFactor;
        float cosFactor;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

} // namespace DrawingBot
