#include "pfm/sketch_catmull_roms_pfm.h"
#include "pfm/sketch_curves_pfm.h"
#include "pfm/sketch_curves.h"
#include "pfm/sketch_lines.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

SketchCatmullRomsPFM::SketchCatmullRomsPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> SketchCatmullRomsPFM::defineSettings() const {
    return SketchCurvesPFM().settingsList();
}

QVector<DrawingGeometry> SketchCatmullRomsPFM::_process(const cv::Mat& image) {
    SketchCurvesPFM sketch;
    for (auto it = m_settings.constBegin(); it != m_settings.constEnd(); ++it) {
        sketch.set(it.key(), it.value().currentValue());
    }
    connect(&sketch, &PathFindingModule::progressUpdate, this, &PathFindingModule::progressUpdate);
    return sketch.process(image);
}
