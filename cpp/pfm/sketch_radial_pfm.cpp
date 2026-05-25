#include "pfm/sketch_radial_pfm.h"
#include "pfm/sketch_curves.h"
#include "pfm/sketch_lines.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

SketchRadialPFM::SketchRadialPFM(QObject* parent) : PathFindingModule(parent) { initSettings(); }

QVector<PFMSetting> SketchRadialPFM::defineSettings() const {
    return { {"lines", "Lines", SettingType::Integer, 1000, QVariant(), 10, 5000, 10, 5000, 10} };
}

QVector<DrawingGeometry> SketchRadialPFM::_process(const cv::Mat& image) {
    int lines = m_settings["lines"].toInt();
    int w = image.cols;
    int h = image.rows;
    float cx = w / 2.0f;
    float cy = h / 2.0f;
    QVector<DrawingGeometry> geoms;
    for (int i = 0; i < lines; ++i) {
        if (isCancelled()) break;
        if (i % 50 == 0) emitProgress(float(i) / lines, geoms.size(), "Sketch Radial...");
        float theta = randUniform(0.0, 2.0 * M_PI);
        Path path;
        float r = 0.0f;
        while (r < std::hypot(cx, cy)) {
            float x = cx + r * std::cos(theta);
            float y = cy + r * std::sin(theta);
            if (x >= 0 && x < w && y >= 0 && y < h) {
                if (image.at<float>(int(y), int(x)) < 200.0f) {
                    path.push_back({x, y});
                } else {
                    if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
                    path.clear();
                }
            }
            r += 2.0f;
        }
        if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}
