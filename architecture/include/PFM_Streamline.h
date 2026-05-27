#pragma once
#include "PFMBase.h"
#include "PFMSettings.h"

namespace DrawingBot {

    // ── Streamline PFMs ─────────────────────────────────────────────────

    class StreamlinesEdgeField : public PFMBase {
    public:
        StreamlinesEdgeFieldSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class StreamlinesFlowField : public PFMBase {
    public:
        StreamlinesFlowFieldSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class StreamlinesSuperformula : public PFMBase {
    public:
        StreamlinesSuperformulaSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

} // namespace DrawingBot
