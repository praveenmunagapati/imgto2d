#pragma once
#include "PFMBase.h"
#include "PFMSettings.h"

namespace DrawingBot {

    // ── Adaptive PFMs ───────────────────────────────────────────────────

    class AdaptiveCircularScribbles : public PFMBase {
    public:
        AdaptiveCircularScribblesSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class AdaptiveShapes : public PFMBase {
    public:
        AdaptiveShapesSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class AdaptiveTriangulation : public PFMBase {
    public:
        AdaptiveTriangulationSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class AdaptiveTree : public PFMBase {
    public:
        AdaptiveTreeSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class AdaptiveStippling : public PFMBase {
    public:
        AdaptiveStipplingSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class AdaptiveDashes : public PFMBase {
    public:
        AdaptiveDashesSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class AdaptiveLetters : public PFMBase {
    public:
        AdaptiveLettersSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class AdaptiveDiagram : public PFMBase {
    public:
        AdaptiveDiagramSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class AdaptiveTSP : public PFMBase {
    public:
        AdaptiveTSPSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

} // namespace DrawingBot
