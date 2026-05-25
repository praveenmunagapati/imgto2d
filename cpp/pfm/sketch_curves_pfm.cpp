#include "pfm/sketch_curves_pfm.h"
#include "pfm/sketch_curves.h"
#include "pfm/pfm_sketch_utils.h"
#include "core/geometry.h"
#include <cmath>
#include <opencv2/imgproc.hpp>

// ---------------------------------------------------------------------------
SketchCurvesPFM::SketchCurvesPFM(QObject* parent) : PathFindingModule(parent) { initSettings(); }

QVector<PFMSetting> SketchCurvesPFM::defineSettings() const {
    auto s = makeSketchCommonSettings();
    s.append({ "curve_smoothness", "Curve Smoothness", SettingType::Integer, 10, {}, 1,50,1,30,1,{},"Curves" });
    s.append({ "curve_alpha",      "Curve Alpha",      SettingType::Number,   0.5,{}, 0.0,1.0,0.0,1.0,0.1,{},"Curves" });
    return s;
}

QVector<DrawingGeometry> SketchCurvesPFM::_process(const cv::Mat& image) {
    return runSketchLoop(this, image, CurveOutputMode::CatmullRom,
                         std::max(1, get("curve_smoothness").toInt()),
                         get("curve_alpha").toDouble());
}
