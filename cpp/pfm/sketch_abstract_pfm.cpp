#include "pfm/sketch_abstract_pfm.h"
#include "pfm/sketch_curves.h"
#include "pfm/sketch_lines.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

SketchAbstractPFM::SketchAbstractPFM(QObject* parent) : PathFindingModule(parent) { initSettings(); }

QVector<PFMSetting> SketchAbstractPFM::defineSettings() const {
    return {
        {"lines", "Lines", SettingType::Integer, 500, QVariant(), 10, 5000, 10, 5000, 10},
        {"length", "Length", SettingType::Number, 150.0, QVariant(), 10.0, 500.0, 10.0, 500.0, 10.0}
    };
}

QVector<DrawingGeometry> SketchAbstractPFM::_process(const cv::Mat& image) {
    int lines = m_settings["lines"].toInt();
    float length = m_settings["length"].toDouble();
    int w = image.cols;
    int h = image.rows;
    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - image.at<float>(y, x);
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;
    
    QVector<DrawingGeometry> geoms;
    for (int i = 0; i < lines; ++i) {
        if (isCancelled()) break;
        if (i % 50 == 0) emitProgress(float(i) / lines, geoms.size(), "Sketch Abstract...");
        int idx = weightedChoice(probs);
        float cx = idx % w;
        float cy = idx / w;
        DrawingGeometry dg;
        dg.path = {{cx - randUniform(0, length), cy - randUniform(0, length)},
                   {cx + randUniform(0, length), cy + randUniform(0, length)}};
        geoms.push_back(dg);
    }
    return geoms;
}
