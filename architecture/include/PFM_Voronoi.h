#pragma once
#include "PFMBase.h"
#include "PFMSettings.h"

namespace DrawingBot {

    // ── Voronoi PFMs ────────────────────────────────────────────────────

    class VoronoiShapes : public PFMBase {
    public:
        VoronoiShapesSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class VoronoiTriangulation : public PFMBase {
    public:
        VoronoiTriangulationSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class VoronoiTree : public PFMBase {
    public:
        VoronoiTreeSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class VoronoiStippling : public PFMBase {
    public:
        VoronoiStipplingSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class VoronoiDashes : public PFMBase {
    public:
        VoronoiDashesSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class VoronoiLetters : public PFMBase {
    public:
        VoronoiLettersSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class VoronoiDiagram : public PFMBase {
    public:
        VoronoiDiagramSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class VoronoiTSP : public PFMBase {
    public:
        VoronoiTSPSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

} // namespace DrawingBot
