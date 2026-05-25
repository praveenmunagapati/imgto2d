#include "pfm/sketch_cubic_beziers_pfm.h"
#include "pfm/sketch_curves.h"
#include "pfm/pfm_sketch_utils.h"
#include "core/geometry.h"
#include <cmath>
#include <opencv2/imgproc.hpp>

// ---------------------------------------------------------------------------
SketchCubicBeziersPFM::SketchCubicBeziersPFM(QObject* parent) : PathFindingModule(parent) { initSettings(); }

QVector<PFMSetting> SketchCubicBeziersPFM::defineSettings() const {
    auto s = makeSketchCommonSettings();
    s.append({ "curve_smoothness", "Curve Smoothness", SettingType::Integer, 20, {}, 1,100,1,60,1,{},"Curves" });
    return s;
}

QVector<DrawingGeometry> SketchCubicBeziersPFM::_process(const cv::Mat& image) {
    return runSketchLoop(this, image, CurveOutputMode::CubicBezier,
                         std::max(1, get("curve_smoothness").toInt()));
}
