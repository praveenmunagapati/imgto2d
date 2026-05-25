#include "pfm/maze_hilbert_pfm.h"
static void hilbert_curve(float x0, float y0, float xi, float xj, float yi, float yj, int n, Path& pts) {
    if (n <= 0) { pts.push_back({x0 + (xi + yi) / 2.0f, y0 + (xj + yj) / 2.0f}); return; }
    hilbert_curve(x0, y0, yi/2, yj/2, xi/2, xj/2, n - 1, pts);
    hilbert_curve(x0 + xi/2, y0 + xj/2, xi/2, xj/2, yi/2, yj/2, n - 1, pts);
    hilbert_curve(x0 + xi/2 + yi/2, y0 + xj/2 + yj/2, xi/2, xj/2, yi/2, yj/2, n - 1, pts);
    hilbert_curve(x0 + xi/2 + yi, y0 + xj/2 + yj, -yi/2, -yj/2, -xi/2, -xj/2, n - 1, pts);
}
#include <opencv2/imgproc.hpp>
#include <map>
#include <queue>
#include <cmath>

MazeHilbertPFM::MazeHilbertPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> MazeHilbertPFM::defineSettings() const {
    return {
        {"order", "Recursion Order", SettingType::Integer, 7, QVariant(), 3, 10, 3, 10, 1},
        {"threshold", "Darkness Threshold", SettingType::Percentage, 50.0, QVariant(), 0.0, 100.0, 0.0, 100.0, 1.0}
    };
}

QVector<DrawingGeometry> MazeHilbertPFM::_process(const cv::Mat& image) {
    int order = m_settings["order"].toInt();
    float thresh = (m_settings["threshold"].toDouble() / 100.0f) * 255.0f;
    Path pts;
    hilbert_curve(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, order, pts);
    
    QVector<DrawingGeometry> geoms;
    Path path;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        if (i % 1000 == 0) emitProgress(float(i) / pts.size(), geoms.size(), "Hilbert...");
        int x = std::clamp(int(pts[i].first * image.cols), 0, image.cols - 1);
        int y = std::clamp(int(pts[i].second * image.rows), 0, image.rows - 1);
        
        if ((255.0f - image.at<float>(y, x)) > thresh) {
            path.push_back({float(x), float(y)});
        } else {
            if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
            path.clear();
        }
    }
    if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    return geoms;
}
