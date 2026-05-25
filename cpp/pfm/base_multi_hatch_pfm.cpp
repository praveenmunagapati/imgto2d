#include "pfm/base_multi_hatch_pfm.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

#include "pfm/hatch_extras.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

BaseMultiHatchPFM::BaseMultiHatchPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> BaseMultiHatchPFM::defineSettings() const {
    return {
        {"spacing", "Spacing", SettingType::Number, 5.0, QVariant(), 1.0, 50.0, 1.0, 50.0, 0.5},
        {"threshold", "Darkness Threshold", SettingType::Percentage, 50.0, QVariant(), 0.0, 100.0, 0.0, 100.0, 1.0}
    };
}

QVector<DrawingGeometry> BaseMultiHatchPFM::_process(const cv::Mat& image) {
    float spacing = m_settings["spacing"].toDouble();
    float thresh = (m_settings["threshold"].toDouble() / 100.0f) * 255.0f;
    std::vector<float> angles = getAngles();
    
    QVector<DrawingGeometry> geoms;
    int w = image.cols;
    int h = image.rows;
    float diag_len = std::hypot(w, h);
    int num_lines = int(diag_len / std::max(1.0f, spacing));
    
    for (float angle : angles) {
        if (isCancelled()) break;
        float rad = angle * M_PI / 180.0f;
        float dx = std::cos(rad);
        float dy = std::sin(rad);
        
        for (int i = 0; i < num_lines; ++i) {
            if (isCancelled()) break;
            if (i % 20 == 0) emitProgress(float(i) / num_lines, geoms.size(), QString("Hatching %1 deg...").arg(angle));
            
            float offset = (i - num_lines / 2.0f) * spacing;
            float cx = w / 2.0f + offset * -dy;
            float cy = h / 2.0f + offset * dx;
            float x1 = cx - dx * diag_len;
            float y1 = cy - dy * diag_len;
            float x2 = cx + dx * diag_len;
            float y2 = cy + dy * diag_len;
            
            Path path;
            int steps = std::max(1, int(diag_len));
            for (int step = 0; step < steps; ++step) {
                float px = x1 + (x2 - x1) * (float(step) / steps);
                float py = y1 + (y2 - y1) * (float(step) / steps);
                if (px >= 0 && px < w && py >= 0 && py < h) {
                    if ((255.0f - image.at<float>(int(py), int(px))) > thresh) {
                        path.push_back({px, py});
                    } else {
                        if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
                        path.clear();
                    }
                }
            }
            if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
        }
    }
    return geoms;
}
