#include "pfm/adaptive_tsppfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <cmath>
#include <map>
#include <queue>
static Path solve_tsp_adaptive(const std::vector<cv::Point2f>& points,
                                std::function<bool()> isCancelledFn) {
    if (points.empty()) return {};
    std::vector<bool> visited(points.size(), false);
    Path path;
    path.reserve(points.size());
    int current = 0;
    visited[0] = true;
    path.push_back({points[0].x, points[0].y});
    int remaining = (int)points.size() - 1;
    while (remaining > 0) {
        if (isCancelledFn()) return {};
        int best_i = -1;
        float best_d = 1e12f;
        for (size_t i = 0; i < points.size(); ++i) {
            if (!visited[i]) {
                float dx = points[i].x - points[current].x;
                float dy = points[i].y - points[current].y;
                float d = dx*dx + dy*dy;
                if (d < best_d) { best_d = d; best_i = (int)i; }
            }
        }
        if (best_i == -1) break;
        visited[best_i] = true;
        path.push_back({points[best_i].x, points[best_i].y});
        current = best_i;
        remaining--;
    }
    return path;
}

// ---------------------------------------------------------------------------

QVector<DrawingGeometry> AdaptiveTSPPFM::_process(const cv::Mat& image) {
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
    if (pts.size() < 2) return {};
    emitProgress(0.3f, 0, "TSP solving...");
    Path path = solve_tsp_adaptive(pts, [this](){ return isCancelled(); });
    if (path.size() < 2) return {};
    return {DrawingGeometry{path, 0}};
}
