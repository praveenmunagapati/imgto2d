#pragma once
#include "pfm/pfm_base.h"
#include <opencv2/imgproc.hpp>
#include "core/geometry.h"

enum class CurveOutputMode { CatmullRom, QuadBezier, CubicBezier };

QVector<PFMSetting> makeSketchCommonSettings();
QVector<DrawingGeometry> runSketchLoop(
    PathFindingModule* pfm,
    const cv::Mat& image,
    CurveOutputMode mode,
    int curveSmooth = 10,
    double curveAlpha = 0.5);
