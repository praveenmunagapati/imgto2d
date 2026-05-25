#include "pfm/spiral_sawtooth_pfm.h"
#include <cmath>

#include <cmath>

SpiralSawtoothPFM::SpiralSawtoothPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> SpiralSawtoothPFM::defineSettings() const {
    return {
        {"pitch", "Pitch", SettingType::Number, 4.0, QVariant(), 1.0, 30.0, 1.0, 30.0, 0.5},
        {"teeth", "Teeth", SettingType::Integer, 8, QVariant(), 2, 64, 2, 64, 1}
    };
}

QVector<DrawingGeometry> SpiralSawtoothPFM::_process(const cv::Mat& image) {
    float pitch = m_settings["pitch"].toDouble();
    int teeth = m_settings["teeth"].toInt();
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
        if (int(theta * 100) % 500 == 0) emitProgress(r / max_r, 0, "Spiral Sawtooth...");
        
        float tooth = std::sin(theta * teeth) * pitch * 0.3f;
        float x = cx + (r + tooth) * std::cos(theta);
        float y = cy + (r + tooth) * std::sin(theta);
        
        path.push_back({x, y});
        theta += 0.15f;
        r = theta * pitch / (2.0f * M_PI);
    }
    
    if (path.size() > 1) {
        DrawingGeometry dg; dg.path = path;
        geoms.push_back(dg);
    }
    return geoms;
}
