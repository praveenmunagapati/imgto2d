#include "pfm/base_tsppfm.h"
#include <opencv2/imgproc.hpp>
#include <map>
#include <queue>
#include <cmath>
static Path solve_tsp_nn(const std::vector<cv::Point2f>& points, std::function<bool()> isCancelledFunc, std::function<void(float)> progressFunc) {
    if (points.empty()) return {};
    
    std::vector<bool> visited(points.size(), false);
    Path path;
    path.reserve(points.size());
    
    int current = 0;
    visited[0] = true;
    path.push_back({points[0].x, points[0].y});
    
    int remaining = points.size() - 1;
    int total = remaining;

    while (remaining > 0) {
        if (isCancelledFunc()) return {};
        if (remaining % 1000 == 0) progressFunc(1.0f - (float(remaining) / total));

        int best_i = -1;
        float best_d = 1e12f;
        float cx = points[current].x;
        float cy = points[current].y;
        
        for (size_t i = 0; i < points.size(); ++i) {
            if (!visited[i]) {
                float dx = points[i].x - cx;
                float dy = points[i].y - cy;
                float d = dx*dx + dy*dy;
                if (d < best_d) {
                    best_d = d;
                    best_i = i;
                }
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

// -------------------------------------------------------------------------
BaseTSPPFM::BaseTSPPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> BaseTSPPFM::defineSettings() const {
    return {
        {"nodes", "Nodes", SettingType::Integer, 2000, QVariant(), 100, 20000, 100, 20000, 100}
    };
}

QVector<DrawingGeometry> BaseTSPPFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int nodes = m_settings["nodes"].toInt();

    std::vector<double> probs = getProbabilities(image);
    if (probs.empty()) return {};
    
    double sum = 0.0;
    for (auto p : probs) sum += p;
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    std::vector<cv::Point2f> points;
    for (int i = 0; i < nodes; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        points.push_back(cv::Point2f(idx % w, idx / w));
    }

    auto path = solve_tsp_nn(points, [this](){ return isCancelled(); }, [this](float p){ emitProgress(0.5f + p * 0.5f, 0, "Solving TSP..."); });
    if (path.size() >= 2) {
        DrawingGeometry dg;
        dg.path = path;
        return {dg};
    }
    return {};
}
