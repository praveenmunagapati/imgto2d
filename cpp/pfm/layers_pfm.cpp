#include "pfm/layers_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <cmath>
#include "pfm/sketch_lines.h"
#include "pfm/stipple_dots.h"

// ---------------------------------------------------------------------------

LayersPFM::LayersPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> LayersPFM::defineSettings() const {
    return {
        {"plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0, QVariant(), 0.1, 2.0, 0.1, 2.0, 0.05},
        {"random_seed",         "Random Seed",         SettingType::Integer, 42,  QVariant(), 0, 999999, 0, 999999, 1},
        {"num_layers",          "Number of Layers",    SettingType::Integer, 4,   QVariant(), 2, 8, 2, 8, 1},
        {"line_density",        "Line Density (%)",    SettingType::Percentage, 40.0, QVariant(), 0, 100, 10, 100, 5},
    };
}

QVector<DrawingGeometry> LayersPFM::_process(const cv::Mat& image) {
    // Optionally resize to plotting resolution
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image.clone();
    }

    int n = m_settings["num_layers"].toInt();
    double lineDensity = m_settings["line_density"].toDouble();
    QVector<DrawingGeometry> geoms;

    // Linearly spaced brightness thresholds
    float step = 200.0f / n;
    for (int layer = 0; layer < n && !isCancelled(); ++layer) {
        float lo = 30.0f + layer * step;
        float hi = lo + step;

        // Build a masked image: only pixels in [lo, hi) brightness range are drawn
        cv::Mat layerImg = cv::Mat::ones(workImg.size(), CV_8UC1) * 255;
        for (int y = 0; y < workImg.rows; ++y) {
            const uchar* src = workImg.ptr<uchar>(y);
            uchar* dst = layerImg.ptr<uchar>(y);
            for (int x = 0; x < workImg.cols; ++x) {
                float v = (float)src[x];
                if (v >= lo && v < hi) dst[x] = src[x];
                // else stays white (255)
            }
        }

        // Run SketchLinesPFM on this layer
        SketchLinesPFM sketch;
        sketch.set("line_density", lineDensity);
        sketch.set("plotting_resolution", 1.0);

        auto layerGeoms = sketch.process(layerImg);
        for (auto& g : layerGeoms) {
            g.penIndex   = layer;
            g.groupIndex = layer;
            geoms.append(g);
        }
        emitProgress(float(layer + 1) / n, geoms.size(),
                     QString("Layer %1/%2...").arg(layer + 1).arg(n));
    }
    return geoms;
}
