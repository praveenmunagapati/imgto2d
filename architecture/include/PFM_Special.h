#pragma once
#include "PFMBase.h"
#include "PFMSettings.h"

namespace DrawingBot {

    // ── Special PFMs ────────────────────────────────────────────────────

    class ECSDrawing : public PFMBase {
    public:
        ECSDrawingSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class SVGConverter : public PFMBase {
    public:
        SVGConverterSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class PenCalibration : public PFMBase {
    public:
        PenCalibrationSettings settings;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

} // namespace DrawingBot
