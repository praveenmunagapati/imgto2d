#include "pfm/base_adaptive_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <cmath>
#include <map>
#include <queue>

// ---------------------------------------------------------------------------
// BaseAdaptivePFM
// ---------------------------------------------------------------------------

BaseAdaptivePFM::BaseAdaptivePFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> BaseAdaptivePFM::defineSettings() const {
    return {
        {"plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0, QVariant(), 0.1, 2.0,   0.1, 2.0,   0.05},
        {"random_seed",         "Random Seed",         SettingType::Integer, 42,  QVariant(), 0,   999999, 0,   999999, 1},
        {"cell_count",          "Cell Count",          SettingType::Integer, 800, QVariant(), 50,  20000,  50,  20000,  50},
        {"lloyd_iterations",    "Lloyd Iterations",    SettingType::Integer, 3,   QVariant(), 0,   20,     0,   10,     1},
        {"min_brightness",      "Min Brightness (%)",  SettingType::Number,  80.0, QVariant(), 0.0, 100.0, 0.0, 100.0, 1.0},
    };
}

std::vector<cv::Point2f> BaseAdaptivePFM::getSeeds(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int cellCount   = m_settings["cell_count"].toInt();
    int lloydIters  = m_settings["lloyd_iterations"].toInt();
    float minBright = (float)(m_settings["min_brightness"].toDouble() / 100.0) * 255.0f;

    // Build darkness weight map
    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    // Initial weighted sampling
    std::vector<cv::Point2f> pts;
    pts.reserve(cellCount);
    for (int i = 0; i < cellCount; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        pts.push_back(cv::Point2f(idx % w, idx / w));
    }

    // Lloyd relaxation (simplified — move toward dark neighbourhood)
    for (int iter = 0; iter < lloydIters && !isCancelled(); ++iter) {
        emitProgress(float(iter) / lloydIters, 0, "Lloyd relaxation...");
        for (auto& pt : pts) {
            int px = (int)pt.x, py = (int)pt.y;
            double bestW = -1.0;
            float bx = pt.x, by = pt.y;
            for (int dy = -5; dy <= 5; ++dy)
                for (int dx = -5; dx <= 5; ++dx) {
                    int nx = px + dx, ny = py + dy;
                    if (nx < 0 || nx >= w || ny < 0 || ny >= h) continue;
                    double ww = probs[ny * w + nx];
                    if (ww > bestW) { bestW = ww; bx = nx; by = ny; }
                }
            pt = {bx, by};
        }
    }

    // Filter by min brightness
    std::vector<cv::Point2f> filtered;
    for (const auto& p : pts) {
        int xi = std::clamp((int)p.x, 0, w - 1);
        int yi = std::clamp((int)p.y, 0, h - 1);
        float bright = image.at<uchar>(yi, xi);
        if (bright <= (255.0f - minBright))  // only dark enough pixels
            filtered.push_back(p);
    }
    return filtered;
}
