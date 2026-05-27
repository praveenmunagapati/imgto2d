#pragma once
#include "PFMBase.h"
#include "PFMSettings.h"

namespace DrawingBot {

    struct DrawingStyle {
        std::string name;
        PFMBase* pfm;         // pointer to nested PFM instance
        int drawingSetIndex;  // references a DrawingSet
        float weight;         // distribution probability
    };

    // ── Composite PFMs ──────────────────────────────────────────────────

    class MosaicRectangles : public PFMBase {
    public:
        MosaicRectanglesSettings settings;
        std::vector<DrawingStyle> drawingStyles;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class MosaicVoronoi : public PFMBase {
    public:
        MosaicVoronoiSettings settings;
        std::vector<DrawingStyle> drawingStyles;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class MosaicTriangulation : public PFMBase {
    public:
        MosaicTriangulationSettings settings;
        std::vector<DrawingStyle> drawingStyles;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class MosaicSegments : public PFMBase {
    public:
        MosaicSegmentsSettings settings;
        std::vector<DrawingStyle> drawingStyles;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class MosaicCustom : public PFMBase {
    public:
        CompositeBaseSettings settings;
        std::vector<DrawingStyle> drawingStyles;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class LayersPFM : public PFMBase {
    public:
        LayersPFMSettings settings;
        std::vector<DrawingStyle> drawingStyles;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

} // namespace DrawingBot
