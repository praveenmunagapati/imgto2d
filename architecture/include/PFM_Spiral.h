#pragma once
#include "PFMBase.h"
#include "PFM_Mixins.h"
#include "PFMSettings.h"

namespace DrawingBot {

    class SpiralBase : public PFMBase {
    public:
        SpiralSettings settings;
        enum class SpiralType { ARCHIMEDEAN, PARABOLIC } spiralType;
        float spiralSize;       // 0-100
        float centreX;          // 0-100
        float centreY;          // 0-100
        float ringSpacing;      // 0-50
        float amplitude;        // 0.01-2.0
        bool  variableVelocity;
        float minVelocity;      // 1.0-360.0
        float maxVelocity;      // 1.0-360.0
        bool  ignoreWhite;
    };

    class SpiralSawtooth : public SpiralBase {
    public:
        bool connectedLines;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class SpiralCircularScribbles : public SpiralBase, public CircularScribblesBase {
    public:
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

} // namespace DrawingBot
