#pragma once
#include "PFMBase.h"
#include "PFM_Voronoi.h"

namespace DrawingBot {

    struct DrawingStyle {
        std::string name;
        PFMBase* pfm;         // pointer to nested PFM instance
        int drawingSetIndex;  // references a DrawingSet
        float weight;         // distribution probability
    };

    class MosaicRectangles : public PFMBase {
    public:
        std::vector<DrawingStyle> drawingStyles;
        bool  drawOutlines;
        bool  squareTiles;
        int   columns;              // 1-64
        int   rows;                 // 1-64
        float columnPaddingPercent; // 1-100
        float rowPaddingPercent;    // 1-100
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class MosaicVoronoi : public VoronoiBase {
    public:
        std::vector<DrawingStyle> drawingStyles;
        bool  drawOutlines;
        int   tileCount;     // -20 to 20
        float offsetCells;   // -20.0 to 20.0
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class MosaicTriangulation : public MosaicVoronoi {
    public:
        bool triangulateCorners;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class MosaicSegments : public MosaicVoronoi {
    public:
        int   segments;     // 1-5000 (SLIC superpixel count)
        int   iterations;   // 1-100 (SLIC accuracy)
        float compactness;  // 1-100
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class MosaicCustom : public PFMBase {
    public:
        std::vector<DrawingStyle> drawingStyles;
        bool drawOutlines;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class LayersPFM : public PFMBase {
    public:
        std::vector<DrawingStyle> drawingStyles;
        bool keepLightenedImage;
        enum class LayerDistribution { NONE, ORDERED_PER_PFM, ORDERED } layerDistribution;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

} // namespace DrawingBot
