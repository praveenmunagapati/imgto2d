#pragma once
#include "PFMBase.h"
#include "PFM_Mixins.h"
#include "PFMSettings.h"

namespace DrawingBot {

    class AdaptiveBase : public PFMBase {
    public:
        AdaptiveSettings settings;
        float minSampleRadius; // 0.1-100.0
        float maxSampleRadius; // 0.1-100.0
        float brightness;      // 0.0-2.0
        float contrast;        // 0.0-2.0
        bool  ignoreWhite;
    };

    class AdaptiveCircularScribbles : public AdaptiveBase, public CircularScribblesBase {
    public:
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class AdaptiveShapes : public AdaptiveBase, public ShapesBase {
    public:
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class AdaptiveTriangulation : public AdaptiveBase {
    public:
        bool triangulateCorners;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class AdaptiveTree : public AdaptiveBase {
    public:
        bool createCurves;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class AdaptiveStippling : public AdaptiveBase {
    public:
        float stippleSize; // 1.0-100.0
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class AdaptiveDashes : public AdaptiveBase, public ShapesBase {
    public:
        float distortion; // 0.0-100.0
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class AdaptiveLetters : public AdaptiveBase, public ShapesBase {
    public:
        LetterParams letterParams;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class AdaptiveDiagram : public AdaptiveBase {
    public:
        enum class VoronoiStyle { 
            CLASSIC, SMOOTH, SHARP, 
            OFFSET_A, OFFSET_B, OFFSET_C 
        } voronoiStyle;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class AdaptiveTSP : public AdaptiveBase {
    public:
        bool mergeTSPPaths;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

} // namespace DrawingBot
