#include "pfm/spiral_circular_scribbles_pfm.h"
#include <cmath>

SpiralCircularScribblesPFM::SpiralCircularScribblesPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> SpiralCircularScribblesPFM::defineSettings() const {
    return {
        {"pitch", "Pitch", SettingType::Number, 3.0, QVariant(), 1.0, 20.0, 1.0, 20.0, 0.5},
        {"wobble", "Wobble", SettingType::Number, 2.0, QVariant(), 0.0, 15.0, 0.0, 15.0, 0.5}
    };
}

QVector<DrawingGeometry> SpiralCircularScribblesPFM::_process(const cv::Mat& image) {
    float pitch = m_settings["pitch"].toDouble();
    float wobble = m_settings["wobble"].toDouble();
    int w = image.cols;
    int h = image.rows;
    float cx = w / 2.0f;
    float cy = h / 2.0f;
    float max_r = std::hypot(cx, cy);
    
    Path path;
    float theta = 0.0f;
    float r = 0.0f;
    
    QVector<DrawingGeometry> geoms;
    while (r < max_r) {
        if (isCancelled()) break;
        if (int(theta * 100) % 500 == 0) emitProgress(r / max_r, 0, "Spiral Circular Scribbles...");
        
        int xi = std::clamp(int(cx + r * std::cos(theta)), 0, w - 1);
        int yi = std::clamp(int(cy + r * std::sin(theta)), 0, h - 1);
        float dark = (255.0f - image.at<float>(yi, xi)) / 255.0f;
        
        float disp = std::sin(theta * 5.0f) * wobble * dark;
        float x = cx + (r + disp) * std::cos(theta);
        float y = cy + (r + disp) * std::sin(theta);
        
        path.push_back({x, y});
        theta += 0.12f;
        r = theta * pitch / (2.0f * M_PI);
    }
    
    if (path.size() > 1) {
        DrawingGeometry dg; dg.path = path;
        geoms.push_back(dg);
    }
    return geoms;
}
