#include "pfm/edge_shading_pfm.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

// -------------------------------------------------------------------------
// EdgeShadingPFM
// -------------------------------------------------------------------------
EdgeShadingPFM::EdgeShadingPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> EdgeShadingPFM::defineSettings() const {
    return {
        {"canny1", "Canny Low", SettingType::Integer, 50, QVariant(), 0, 255, 0, 255, 5},
        {"canny2", "Canny High", SettingType::Integer, 150, QVariant(), 0, 255, 0, 255, 5},
        {"spacing", "Hatch Spacing", SettingType::Number, 4.0, QVariant(), 1.0, 30.0, 1.0, 30.0, 0.5}
    };
}

QVector<DrawingGeometry> EdgeShadingPFM::_process(const cv::Mat& image) {
    int canny1 = m_settings["canny1"].toInt();
    int canny2 = m_settings["canny2"].toInt();
    float spacing = m_settings["spacing"].toDouble();
    
    cv::Mat img8u;
    image.convertTo(img8u, CV_8UC1);
    
    cv::Mat edges;
    cv::Canny(img8u, edges, canny1, canny2);
    
    cv::Mat combined;
    cv::Mat edgesF;
    edges.convertTo(edgesF, CV_32F);
    cv::addWeighted(image, 0.5, edgesF, 0.5, 0.0, combined);
    
    QVector<DrawingGeometry> geoms;
    int w = combined.cols;
    int h = combined.rows;
    float diag_len = std::hypot(w, h);
    int num_lines = int(diag_len / std::max(1.0f, spacing));
    
    float rad = 45.0f * M_PI / 180.0f;
    float dx = std::cos(rad);
    float dy = std::sin(rad);
    float depth = 1.0f;
    float thresh = (80.0f / 100.0f) * 255.0f;
    
    for (int i = 0; i < num_lines; ++i) {
        if (isCancelled()) break;
        if (i % 20 == 0) emitProgress(float(i) / num_lines, geoms.size(), "Edge Shading...");
        
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
                float val = combined.at<float>(int(py), int(px));
                float brightness = val / 255.0f;
                if ((255.0f - val) > thresh) {
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
