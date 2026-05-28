#pragma once
#include <vector>
#include <opencv2/core.hpp>

namespace DrawingBot {

    struct PlotPath {
        std::vector<cv::Point2f> points;
        int penIndex = 0;
    };

    class PFMBase {
    public:
        virtual ~PFMBase() = default;

        // Shared Universal Parameters
        float plottingResolution = 1.0f; // 0.1 to 1.0 (safe), values > 1.0 allowed
        int   randomSeed = 0;

        // The core interface for all Path Finding Modules
        virtual std::vector<PlotPath> generate(const cv::Mat& referenceImage) = 0;
    };

} // namespace DrawingBot
