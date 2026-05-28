#pragma once
#include "PFMBase.h"
#include "PFMSettings.h"

namespace DrawingBot {

    // ── Hatch PFMs ──────────────────────────────────────────────────────

    class HatchSawtooth : public PFMBase {
    public:
        HatchSawtoothSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class HatchCircularScribbles : public PFMBase {
    public:
        HatchCircularScribblesSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

} // namespace DrawingBot
