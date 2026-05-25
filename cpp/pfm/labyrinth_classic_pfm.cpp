#include "pfm/labyrinth_classic_pfm.h"
#include <opencv2/imgproc.hpp>
#include <map>
#include <queue>
#include <cmath>

LabyrinthClassicPFM::LabyrinthClassicPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> LabyrinthClassicPFM::defineSettings() const {
    return {
        {"spacing", "Spiral Spacing", SettingType::Number, 10.0, QVariant(), 2.0, 50.0, 2.0, 50.0, 1.0},
        {"wobble", "Wobble Factor", SettingType::Number, 0.0, QVariant(), 0.0, 10.0, 0.0, 10.0, 0.5}
    };
}

QVector<DrawingGeometry> LabyrinthClassicPFM::_process(const cv::Mat& image) {
    float spacing = m_settings["spacing"].toDouble();
    float wobble = m_settings["wobble"].toDouble();
    float cx = image.cols / 2.0f;
    float cy = image.rows / 2.0f;
    float max_r = std::hypot(cx, cy);
    float theta = 0.0f;
    float b = spacing / (2.0f * M_PI);
    
    QVector<DrawingGeometry> geoms;
    Path path;
    int i = 0;
    while (true) {
        if (isCancelled()) break;
        float r = b * theta;
        if (r > max_r) break;
        
        float x = cx + r * std::cos(theta);
        float y = cy + r * std::sin(theta);
        int ix = int(x), iy = int(y);
        
        if (ix >= 0 && ix < image.cols && iy >= 0 && iy < image.rows) {
            float brightness = image.at<float>(iy, ix) / 255.0f;
            float wx = x + (std::cos(theta * 10.0f) * wobble * (1.0f - brightness));
            float wy = y + (std::sin(theta * 10.0f) * wobble * (1.0f - brightness));
            path.push_back({wx, wy});
        } else {
            if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
            path.clear();
        }
        float step = std::max(0.01f, std::min(0.5f, 2.0f / std::max(1.0f, r)));
        theta += step;
        if (i++ % 5000 == 0) emitProgress(r / max_r, geoms.size(), "Labyrinth...");
    }
    if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    return geoms;
}
