#include "pfm/adaptive_circular_scribbles_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <cmath>
#include <map>
#include <queue>

// ---------------------------------------------------------------------------
// AdaptiveCircularScribblesPFM
// ---------------------------------------------------------------------------

AdaptiveCircularScribblesPFM::AdaptiveCircularScribblesPFM(QObject* p)
    : BaseAdaptivePFM(p) {}

QVector<PFMSetting> AdaptiveCircularScribblesPFM::defineSettings() const {
    auto s = BaseAdaptivePFM::defineSettings();
    s.append({"scribble_turns", "Scribble Turns", SettingType::Number, 2.0, QVariant(), 0.5, 8.0, 0.5, 8.0, 0.5});
    s.append({"radius_scale",   "Radius Scale",   SettingType::Number, 3.0, QVariant(), 0.5, 20.0, 0.5, 20.0, 0.5});
    return s;
}

QVector<DrawingGeometry> AdaptiveCircularScribblesPFM::_process(const cv::Mat& image) {
    // Apply plotting_resolution scaling
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
    double turns   = m_settings["scribble_turns"].toDouble();
    double rscale  = m_settings["radius_scale"].toDouble();
    QVector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        if (i % 50 == 0) emitProgress(float(i) / pts.size(), (int)geoms.size(), "Adaptive scribbles...");
        float cx = pts[i].x, cy = pts[i].y;
        int steps = std::max(12, (int)(turns * 24));
        Path path;
        for (int s = 0; s <= steps; ++s) {
            double t = (double)s / steps * turns * M_PI * 2.0;
            path.push_back({cx + std::cos(t) * (float)rscale,
                            cy + std::sin(t) * (float)rscale});
        }
        geoms.push_back(DrawingGeometry{path, 0});
    }
    return geoms;
}
