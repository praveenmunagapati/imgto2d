#pragma once
#include "pfm/pfm_base.h"
#include <cmath>

static inline Path trace_streamline(const cv::Mat& fx, const cv::Mat& fy, float x, float y, int max_len, float step_size) {
    Path path;
    int w = fx.cols;
    int h = fx.rows;
    for (int i = 0; i < max_len; ++i) {
        int xi = std::clamp(int(x), 0, w - 1);
        int yi = std::clamp(int(y), 0, h - 1);
        path.push_back({x, y});
        
        float vx = fx.at<float>(yi, xi);
        float vy = fy.at<float>(yi, xi);
        float mag = std::hypot(vx, vy);
        if (mag < 1e-6f) break;
        
        x += (vx / mag) * step_size;
        y += (vy / mag) * step_size;
        if (x < 0 || x >= w || y < 0 || y >= h) break;
    }
    return path;
}

class BaseStreamlinePFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit BaseStreamlinePFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};
