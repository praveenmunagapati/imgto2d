#include "pfm/sketch_scribble_pfm.h"
#include "pfm/sketch_curves.h"
#include "pfm/sketch_lines.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

SketchScribblePFM::SketchScribblePFM(QObject* parent) : PathFindingModule(parent) { initSettings(); }

QVector<PFMSetting> SketchScribblePFM::defineSettings() const {
    return {
        {"lines", "Lines", SettingType::Integer, 1000, QVariant(), 10, 5000, 10, 5000, 10},
        {"length", "Length", SettingType::Number, 50.0, QVariant(), 5.0, 200.0, 5.0, 200.0, 1.0}
    };
}

QVector<DrawingGeometry> SketchScribblePFM::_process(const cv::Mat& image) {
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
        if (i % 50 == 0) emitProgress(float(i) / lines, geoms.size(), "Sketch Scribble...");
        int idx = weightedChoice(probs);
        float cx = idx % w;
        float cy = idx / w;
        Path path;
        int pts = int(randUniform(3, 8));
        for (int j = 0; j < pts; ++j) {
            path.push_back({cx + randUniform(-length, length), cy + randUniform(-length, length)});
        }
        DrawingGeometry dg; dg.path = catmull_rom_chain(path); geoms.push_back(dg);
    }
    return geoms;
}
