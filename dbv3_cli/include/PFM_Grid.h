#pragma once
#include "PFMBase.h"
#include "PFMSettings.h"

namespace DrawingBot {

    // ── Grid PFMs ───────────────────────────────────────────────────────

    class GridShapes : public PFMBase {
    public:
        GridShapesSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class GridDashes : public PFMBase {
    public:
        GridDashesSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class GridLetters : public PFMBase {
    public:
        GridLettersSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

} // namespace DrawingBot
