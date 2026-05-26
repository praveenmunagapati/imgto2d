#pragma once
#include "pfm/pfm_base.h"
#include <functional>
#include <algorithm>

static inline Path solve_tsp_nn(const std::vector<cv::Point2f>& points,
                                std::function<bool()> isCancelledFunc = []{ return false; },
                                std::function<void(float)> progressFunc = [](float){}) {
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
        if (remaining % 1000 == 0) progressFunc(1.0f - (float(remaining) / std::max(1, total)));

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

class BaseTSPPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit BaseTSPPFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    virtual std::vector<double> getProbabilities(const cv::Mat& image) = 0;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
