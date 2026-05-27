#pragma once
#include "PFMBase.h"
#include "PFM_Mixins.h"
#include "PFMSettings.h"

namespace DrawingBot {

    class HatchBase : public PFMBase {
    public:
        HatchSettings settings;
        float lineSpacing; // 2.0-50.0
        float angle;       // 2.0-50.0
        bool  crosshatch;
        bool  linkEnds;
    };

    class HatchSawtooth : public HatchBase {
    public:
        float amplitude;    // 0.01-2.0
        float minVelocity;  // 1.0-360.0
        float maxVelocity;  // 1.0-360.0
        float curveTension; // 0.0-1.0
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class HatchCircularScribbles : public HatchBase, public CircularScribblesBase {
    public:
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

} // namespace DrawingBot
