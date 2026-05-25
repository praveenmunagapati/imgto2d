#include "pfm/sketch_quad_beziers_pfm.h"
#include "pfm/sketch_curves.h"
#include "pfm/pfm_sketch_utils.h"
#include "core/geometry.h"
#include <cmath>
#include <opencv2/imgproc.hpp>

// ---------------------------------------------------------------------------
SketchQuadBeziersPFM::SketchQuadBeziersPFM(QObject* parent) : PathFindingModule(parent) { initSettings(); }

QVector<PFMSetting> SketchQuadBeziersPFM::defineSettings() const {
    auto s = makeSketchCommonSettings();
    s.append({ "curve_smoothness", "Curve Smoothness", SettingType::Integer, 20, {}, 1,100,1,60,1,{},"Curves" });
    return s;
}

QVector<DrawingGeometry> SketchQuadBeziersPFM::_process(const cv::Mat& image) {
    return runSketchLoop(this, image, CurveOutputMode::QuadBezier,
                         std::max(1, get("curve_smoothness").toInt()));
}
