#include "pfm/adaptive_dashes_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <cmath>
#include <map>
#include <queue>

// ---------------------------------------------------------------------------
// AdaptiveDashesPFM
// ---------------------------------------------------------------------------

QVector<DrawingGeometry> AdaptiveDashesPFM::_process(const cv::Mat& image) {
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
    QVector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        float cx = pts[i].x, cy = pts[i].y;
        float angle  = (float)randUniform(0.0, M_PI);
        float length = (float)randUniform(3.0, 12.0);
        Path path = {
            {cx, cy},
            {cx + std::cos(angle) * length, cy + std::sin(angle) * length}
        };
        geoms.push_back(DrawingGeometry{path, 0});
    }
    emitProgress(1.0f, (int)geoms.size(), "Adaptive dashes done");
    return geoms;
}
