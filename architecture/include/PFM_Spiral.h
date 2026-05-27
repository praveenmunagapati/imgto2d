#pragma once
#include "PFMBase.h"
#include "PFMSettings.h"

namespace DrawingBot {

    // ── Spiral PFMs ─────────────────────────────────────────────────────

    class SpiralSawtooth : public PFMBase {
    public:
        SpiralSawtoothSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class SpiralCircularScribbles : public PFMBase {
    public:
        SpiralCircularScribblesSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

} // namespace DrawingBot
