#pragma once
#include "PFMBase.h"
#include "PFM_Mixins.h"
#include "PFMSettings.h"

namespace DrawingBot {

    class LBGBase : public PFMBase {
    public:
        LBGSettings settings;
    };

    class LBGCircularScribbles : public LBGBase, public CircularScribblesBase { public: std::vector<PlotPath> generate(const cv::Mat& ref) override; };
    class LBGShapes : public LBGBase, public ShapesBase { public: std::vector<PlotPath> generate(const cv::Mat& ref) override; };
    class LBGTriangulation : public LBGBase { public: bool triangulateCorners; std::vector<PlotPath> generate(const cv::Mat& ref) override; };
    class LBGTree : public LBGBase { public: bool createCurves; std::vector<PlotPath> generate(const cv::Mat& ref) override; };
    class LBGStippling : public LBGBase { public: float stippleSize; std::vector<PlotPath> generate(const cv::Mat& ref) override; };
    class LBGDashes : public LBGBase, public ShapesBase { public: float distortion; std::vector<PlotPath> generate(const cv::Mat& ref) override; };
    class LBGLetters : public LBGBase, public ShapesBase { public: LetterParams letterParams; std::vector<PlotPath> generate(const cv::Mat& ref) override; };
    
    class LBGDiagram : public LBGBase {
    public:
        enum class VoronoiStyle { CLASSIC, SMOOTH, SHARP, OFFSET_A, OFFSET_B, OFFSET_C } voronoiStyle;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };
    
    class LBGTSP : public LBGBase { public: bool mergeTSPPaths; std::vector<PlotPath> generate(const cv::Mat& ref) override; };
    class LBGQuadTiles : public LBGBase { public: std::vector<PlotPath> generate(const cv::Mat& ref) override; };

} // namespace DrawingBot
