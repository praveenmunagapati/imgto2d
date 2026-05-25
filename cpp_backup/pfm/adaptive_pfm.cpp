#include "pfm/adaptive_pfm.h"
#include <opencv2/imgproc.hpp>
#include "core/geometry.h"
#include <cmath>
#include <iostream>

AdaptiveStipplingPFM::AdaptiveStipplingPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> AdaptiveStipplingPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0,  {}, 0.05,2.0,  0.1, 1.0, 0.05 },
        { "random_seed",         "Random Seed",         SettingType::Integer, 42,   {}, 0,  999999, 0,   999999, 1 },
        { "cell_count",          "Cell Count",          SettingType::Integer, 3000, {}, 100,50000,  100, 20000, 100 },
        { "lloyd_iterations",    "Lloyd Iterations",    SettingType::Integer, 3,    {}, 0,  20,     0,   10,     1 },
        { "min_brightness",      "Min Brightness",      SettingType::Percentage, 80.0, {}, 0.0, 100.0, 0.0, 100.0, 1.0 },
        { "dot_radius",          "Dot Radius",          SettingType::Number,  1.0,  {}, 0.2,8.0,   0.2, 5.0,  0.1 },
    };
}

QVector<DrawingGeometry> AdaptiveStipplingPFM::_process(const cv::Mat& image) {
    // Apply plotting resolution
    cv::Mat workImg;
    double plotRes = get("plotting_resolution").toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image.clone();
    }
    const int W = workImg.cols, H = workImg.rows;

    const int    cellCount     = get("cell_count").toInt();
    const int    lloydIter     = get("lloyd_iterations").toInt();
    const double minBrightPct  = get("min_brightness").toDouble() / 100.0;
    const double dotRadius     = get("dot_radius").toDouble();

    // Build darkness weights
    cv::Mat imgF;
    workImg.convertTo(imgF, CV_32F);
    std::vector<double> weights(W * H);
    double totalW = 0.0;
    const float* fp = imgF.ptr<float>(0);
    for (int i = 0; i < W * H; ++i) {
        double d = std::max(0.0, 255.0 - (double)fp[i]);
        weights[i] = d;
        totalW += d;
    }
    if (totalW < 1.0) return {};

    // Initial sample (weighted)
    std::vector<std::pair<double,double>> points(cellCount);
    for (int i = 0; i < cellCount; ++i) {
        int idx = weightedChoice(weights);
        points[i] = { (double)(idx % W), (double)(idx / W) };
    }

    // Lloyd relaxation (simplified: re-weight toward local mean)
    for (int iter = 0; iter < lloydIter && !isCancelled(); ++iter) {
        // For each point, move slightly toward the darkest nearby pixel
        for (auto& pt : points) {
            int px = (int)pt.first, py = (int)pt.second;
            // Sample a small neighborhood
            double bestW = -1.0;
            double bx = pt.first, by = pt.second;
            for (int dy = -5; dy <= 5; ++dy)
                for (int dx = -5; dx <= 5; ++dx) {
                    int nx = px + dx, ny = py + dy;
                    if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;
                    double w = weights[ny * W + nx];
                    if (w > bestW) { bestW = w; bx = nx; by = ny; }
                }
            pt = { bx, by };
        }
    }

    // Filter by brightness (min_brightness removes dots in very bright areas)
    double minBrightAbs = minBrightPct * 255.0;

    QVector<DrawingGeometry> geometries;
    geometries.reserve(cellCount);

    for (int i = 0; i < (int)points.size() && !isCancelled(); ++i) {
        double cx = points[i].first, cy = points[i].second;
        int xi = (int)cx, yi = (int)cy;
        if (xi < 0 || xi >= W || yi < 0 || yi >= H) continue;
        double bright = fp[yi * W + xi];
        if (bright > minBrightAbs) continue;  // skip bright areas

        Path circle = generate_circle(cx, cy, dotRadius, 8);
        geometries.append(DrawingGeometry{ circle, 0 });

        if (i % 500 == 0)
            emitProgress((float)i / points.size(), geometries.size(), "Adaptive stippling...");
    }
    return geometries;
}
