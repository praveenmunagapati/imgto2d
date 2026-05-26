#pragma once
#include "pfm/pfm_base.h"
#include <cmath>

static inline float nearest_seed_radius(float cx, float cy, const std::vector<cv::Point2f>& points) {
    float best = 1e12f;
    for (const auto& pt : points) {
        float dx = pt.x - cx;
        float dy = pt.y - cy;
        if (std::abs(dx) < 1e-6f && std::abs(dy) < 1e-6f) continue;
        float d = std::hypot(dx, dy);
        if (d < best) best = d;
    }
    return best < 1e11f ? best * 0.45f : 8.0f;
}

class BaseVoronoiExtraPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit BaseVoronoiExtraPFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    std::vector<cv::Point2f> getSeeds(const cv::Mat& image);
};
