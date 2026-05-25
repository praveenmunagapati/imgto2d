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

HatchSawtoothPFM::HatchSawtoothPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> HatchSawtoothPFM::defineSettings() const {
    return {
        {"angle", "Angle", SettingType::Number, 45.0, QVariant(), -90.0, 90.0, -90.0, 90.0, 5.0},
        {"spacing", "Spacing", SettingType::Number, 5.0, QVariant(), 1.0, 50.0, 1.0, 50.0, 0.5},
        {"threshold", "Darkness Threshold", SettingType::Percentage, 50.0, QVariant(), 0.0, 100.0, 0.0, 100.0, 1.0},
        {"tooth_depth", "Tooth Depth", SettingType::Number, 2.0, QVariant(), 0.5, 15.0, 0.5, 15.0, 0.5}
    };
}

QVector<DrawingGeometry> HatchSawtoothPFM::_process(const cv::Mat& image) {
    float angle = m_settings["angle"].toDouble();
    float spacing = m_settings["spacing"].toDouble();
    float thresh = (m_settings["threshold"].toDouble() / 100.0f) * 255.0f;
    float depth = m_settings["tooth_depth"].toDouble();
    
    QVector<DrawingGeometry> geoms;
    int w = image.cols;
    int h = image.rows;
    float diag_len = std::hypot(w, h);
    int num_lines = int(diag_len / std::max(1.0f, spacing));
    
    float rad = angle * M_PI / 180.0f;
    float dx = std::cos(rad);
    float dy = std::sin(rad);
    
    for (int i = 0; i < num_lines; ++i) {
        if (isCancelled()) break;
        if (i % 20 == 0) emitProgress(float(i) / num_lines, geoms.size(), "Sawtooth Hatching...");
        
        float offset = (i - num_lines / 2.0f) * spacing;
        float cx = w / 2.0f + offset * -dy;
        float cy = h / 2.0f + offset * dx;
        float x1 = cx - dx * diag_len;
        float y1 = cy - dy * diag_len;
        float x2 = cx + dx * diag_len;
        float y2 = cy + dy * diag_len;
        
        Path path;
        int steps = std::max(1, int(diag_len));
        bool up = true;
        for (int step = 0; step < steps; step += 3) {
            float px = x1 + (x2 - x1) * (float(step) / steps);
            float py = y1 + (y2 - y1) * (float(step) / steps);
            
            if (px >= 0 && px < w && py >= 0 && py < h) {
                float brightness = image.at<float>(int(py), int(px)) / 255.0f;
                if ((255.0f - image.at<float>(int(py), int(px))) > thresh) {
                    float local_depth = depth * (1.0f - brightness);
                    float wx = px + (up ? -dy : dy) * local_depth;
                    float wy = py + (up ? dx : -dx) * local_depth;
                    path.push_back({wx, wy});
                    up = !up;
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
