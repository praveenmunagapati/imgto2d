#pragma once
#include "PFMBase.h"
#include "PFM_Mixins.h"
#include "PFMSettings.h"

namespace DrawingBot {

    class GridBase : public PFMBase {
    public:
        GridSettings settings;
        bool  uniformSpacing;
        float gridXSpacing;      // 1.0-32.0
        float gridYSpacing;      // 1.0-32.0
        float shapeScale;        // 0.01-4.0
        float randOffsetX;       // 0.0-64.0
        float randOffsetY;       // 0.0-64.0
        bool  interleave;
        float brightness;        // 0.0-2.0
        float contrast;          // 0.0-2.0
        float threshold;         // 0.0-100.0
        float thresholdFeather;  // 0.0-100.0
        bool  concentricFills;
        float convergence;       // 0.0-100.0
    };

    class GridShapes : public GridBase, public ShapesBase {
    public:
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class GridDashes : public GridBase, public ShapesBase {
    public:
        float distortion; // 0-100
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class GridLetters : public GridBase, public ShapesBase {
    public:
        LetterParams letterParams;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

} // namespace DrawingBot
