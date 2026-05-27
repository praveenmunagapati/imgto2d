#pragma once
#include "PFMBase.h"
#include "PFM_Mixins.h"
#include "PFMSettings.h"

namespace DrawingBot {

    class VoronoiBase : public PFMBase {
    public:
        VoronoiSettings settings;
    };

    class VoronoiShapes : public VoronoiBase, public ShapesBase { public: std::vector<PlotPath> generate(const cv::Mat& ref) override; };
    class VoronoiTriangulation : public VoronoiBase { public: bool triangulateCorners; std::vector<PlotPath> generate(const cv::Mat& ref) override; };
    class VoronoiTree : public VoronoiBase { public: bool createCurves; std::vector<PlotPath> generate(const cv::Mat& ref) override; };
    class VoronoiStippling : public VoronoiBase { public: float stippleSize; std::vector<PlotPath> generate(const cv::Mat& ref) override; };
    class VoronoiDashes : public VoronoiBase, public ShapesBase { public: float distortion; std::vector<PlotPath> generate(const cv::Mat& ref) override; };
    class VoronoiLetters : public VoronoiBase, public ShapesBase { public: LetterParams letterParams; std::vector<PlotPath> generate(const cv::Mat& ref) override; };
    
    class VoronoiDiagram : public VoronoiBase {
    public:
        enum class VoronoiStyle { CLASSIC, SMOOTH, SHARP, OFFSET_A, OFFSET_B, OFFSET_C } voronoiStyle;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };
    
    class VoronoiTSP : public VoronoiBase { public: bool mergeTSPPaths; std::vector<PlotPath> generate(const cv::Mat& ref) override; };

} // namespace DrawingBot
