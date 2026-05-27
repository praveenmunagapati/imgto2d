#pragma once
#include "PFMBase.h"
#include "PFMSettings.h"

namespace DrawingBot {

    // ── Sketch PFMs ─────────────────────────────────────────────────────

    class SketchLines : public PFMBase {
    public:
        SketchLinesSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class SketchCurves : public PFMBase {
    public:
        SketchCurvesSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class SketchSquares : public PFMBase {
    public:
        SketchSquaresSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class SketchQuadBeziers : public PFMBase {
    public:
        SketchQuadBeziersSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class SketchCubicBeziers : public PFMBase {
    public:
        SketchCubicBeziersSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class SketchCatmullRoms : public PFMBase {
    public:
        SketchCatmullRomsSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class SketchShapes : public PFMBase {
    public:
        SketchShapesSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class SketchSobelEdges : public PFMBase {
    public:
        SketchSobelEdgesSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class SketchWaves : public PFMBase {
    public:
        SketchWavesSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class SketchFlowField : public PFMBase {
    public:
        SketchFlowFieldSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class SketchSuperformula : public PFMBase {
    public:
        SketchSuperformulaSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class SketchSweepingCurves : public PFMBase {
    public:
        SketchSweepingCurvesSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

} // namespace DrawingBot
