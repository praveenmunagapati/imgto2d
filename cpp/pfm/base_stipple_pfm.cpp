#include "pfm/base_stipple_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

#include "pfm/stipple_extras.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

BaseStipplePFM::BaseStipplePFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> BaseStipplePFM::defineSettings() const {
    return {
        {"num_shapes", "Number of Shapes", SettingType::Integer, 5000, QVariant(), 100, 50000, 100, 50000, 100},
        {"shape_size", "Shape Size", SettingType::Number, 1.0, QVariant(), 0.1, 10.0, 0.1, 10.0, 0.1}
    };
}

QVector<DrawingGeometry> BaseStipplePFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int num_shapes = m_settings["num_shapes"].toInt();
    double size = m_settings["shape_size"].toDouble();

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

    QVector<DrawingGeometry> geoms;
    for (int i = 0; i < num_shapes; ++i) {
        if (isCancelled()) break;
        if (i % 500 == 0) emitProgress(float(i) / num_shapes, geoms.size(), "Stippling...");

        int idx = weightedChoice(probs);
        int cx = idx % w;
        int cy = idx / w;
        double local = probs[idx] * sum / 255.0; // recover brightness
        double r = size * (0.3 + 0.7 * local);

        DrawingGeometry dg;
        dg.path = generateShape(cx, cy, r);
        geoms.push_back(dg);
    }
    return geoms;
}
