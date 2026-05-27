#pragma once
#include "PFMBase.h"
#include "PFMSettings.h"

namespace DrawingBot {

    // ── LBG PFMs ────────────────────────────────────────────────────────

    class LBGCircularScribbles : public PFMBase {
    public:
        LBGCircularScribblesSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class LBGShapes : public PFMBase {
    public:
        LBGShapesSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class LBGTriangulation : public PFMBase {
    public:
        LBGTriangulationSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class LBGTree : public PFMBase {
    public:
        LBGTreeSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class LBGStippling : public PFMBase {
    public:
        LBGStipplingSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class LBGDashes : public PFMBase {
    public:
        LBGDashesSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class LBGLetters : public PFMBase {
    public:
        LBGLettersSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class LBGDiagram : public PFMBase {
    public:
        LBGDiagramSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class LBGQuadTiles : public PFMBase {
    public:
        LBGQuadTilesSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class LBGTSP : public PFMBase {
    public:
        LBGTSPSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

} // namespace DrawingBot
