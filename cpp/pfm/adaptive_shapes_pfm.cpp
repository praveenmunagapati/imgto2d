#include "pfm/adaptive_shapes_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <cmath>
#include <map>
#include <queue>

// ---------------------------------------------------------------------------
// AdaptiveShapesPFM
// ---------------------------------------------------------------------------

AdaptiveShapesPFM::AdaptiveShapesPFM(QObject* p) : BaseAdaptivePFM(p) {}

QVector<PFMSetting> AdaptiveShapesPFM::defineSettings() const {
    auto s = BaseAdaptivePFM::defineSettings();
    s.append({"shape_size", "Shape Size", SettingType::Number, 4.0, QVariant(), 1.0, 30.0, 1.0, 30.0, 0.5});
    return s;
}

QVector<DrawingGeometry> AdaptiveShapesPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image;
    }
    auto pts = getSeeds(workImg);
    float size = (float)m_settings["shape_size"].toDouble();
    QVector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        if (i % 50 == 0) emitProgress(float(i) / pts.size(), (int)geoms.size(), "Adaptive shapes...");
        float cx = pts[i].x, cy = pts[i].y;
        Path path;
        int shapeType = i % 4;
        switch (shapeType) {
            case 0: path = generate_circle(cx, cy, size, 12); break;
            case 1: path = generate_polygon(cx, cy, size, 4, 45); break;
            case 2: path = generate_polygon(cx, cy, size, 3, 30); break;
            case 3: path = generate_polygon(cx, cy, size, 6, 0); break;
        }
        geoms.push_back(DrawingGeometry{path, 0});
    }
    return geoms;
}
