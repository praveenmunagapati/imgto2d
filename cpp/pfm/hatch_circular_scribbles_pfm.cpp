#include "pfm/hatch_circular_scribbles_pfm.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

HatchCircularScribblesPFM::HatchCircularScribblesPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> HatchCircularScribblesPFM::defineSettings() const {
    return {
        {"spacing", "Ring Spacing", SettingType::Number, 6.0, QVariant(), 2.0, 40.0, 2.0, 40.0, 0.5},
        {"threshold", "Darkness Threshold", SettingType::Percentage, 50.0, QVariant(), 0.0, 100.0, 0.0, 100.0, 1.0},
        {"turns", "Turns per Ring", SettingType::Number, 1.5, QVariant(), 0.5, 4.0, 0.5, 4.0, 0.1}
    };
}

QVector<DrawingGeometry> HatchCircularScribblesPFM::_process(const cv::Mat& image) {
    float spacing = m_settings["spacing"].toDouble();
    float thresh = (m_settings["threshold"].toDouble() / 100.0f) * 255.0f;
    float turns = m_settings["turns"].toDouble();
    
    QVector<DrawingGeometry> geoms;
    float cx = image.cols / 2.0f;
    float cy = image.rows / 2.0f;
    float max_r = std::hypot(cx, cy);
    int num_rings = int(max_r / spacing);
    
    for (int r_idx = 1; r_idx <= num_rings; ++r_idx) {
        if (isCancelled()) break;
        emitProgress(float(r_idx) / num_rings, geoms.size(), "Circular Scribbles...");
        
        float base_r = r_idx * spacing;
        Path path;
        int steps = std::max(36, int(2.0f * M_PI * base_r * turns));
        for (int step = 0; step < steps; ++step) {
            float theta = (float(step) / steps) * 2.0f * M_PI * turns;
            float r = base_r + std::sin(theta * (base_r / 2.0f)) * (spacing * 0.8f);
            
            float px = cx + r * std::cos(theta);
            float py = cy + r * std::sin(theta);
            
            if (px >= 0 && px < image.cols && py >= 0 && py < image.rows) {
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
    return geoms;
}
