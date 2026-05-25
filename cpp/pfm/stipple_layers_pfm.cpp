#include "pfm/stipple_layers_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <cmath>
#include "pfm/sketch_lines.h"
#include "pfm/stipple_dots.h"

// ---------------------------------------------------------------------------

StippleLayersPFM::StippleLayersPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> StippleLayersPFM::defineSettings() const {
    return {
        {"plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0,  QVariant(), 0.1, 2.0,   0.1,   2.0,   0.05},
        {"random_seed",         "Random Seed",         SettingType::Integer, 42,   QVariant(), 0,   999999, 0,     999999, 1},
        {"layers",              "Layers",              SettingType::Integer, 4,    QVariant(), 2,   8,      2,     8,      1},
        {"dots_per_layer",      "Dots per Layer",      SettingType::Integer, 1500, QVariant(), 100, 20000,  100,   20000,  100},
    };
}

QVector<DrawingGeometry> StippleLayersPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image.clone();
    }

    int n = m_settings["layers"].toInt();
    int dotsPerLayer = m_settings["dots_per_layer"].toInt();
    QVector<DrawingGeometry> geoms;

    float step = 255.0f / n;
    for (int layer = 0; layer < n && !isCancelled(); ++layer) {
        float lo = layer * step;
        float hi = (layer + 1) * step;

        cv::Mat layerImg = cv::Mat::ones(workImg.size(), CV_8UC1) * 255;
        for (int y = 0; y < workImg.rows; ++y) {
            const uchar* src = workImg.ptr<uchar>(y);
            uchar* dst = layerImg.ptr<uchar>(y);
            for (int x = 0; x < workImg.cols; ++x) {
                float v = (float)src[x];
                if (v >= lo && v < hi) dst[x] = src[x];
            }
        }

        StippleDotsPFM stipple;
        stipple.set("num_dots", dotsPerLayer);
        stipple.set("dot_size", 1.0 + layer * 0.3);

        auto layerGeoms = stipple.process(layerImg);
        for (auto& g : layerGeoms) {
            g.penIndex = layer;
            geoms.append(g);
        }
        emitProgress(float(layer + 1) / n, geoms.size(),
                     QString("Stipple layer %1/%2...").arg(layer + 1).arg(n));
    }
    return geoms;
}
