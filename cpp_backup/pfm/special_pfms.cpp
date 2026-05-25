#include "pfm/special_pfms.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

// -------------------------------------------------------------------------
// ContourPathsPFM
// -------------------------------------------------------------------------
ContourPathsPFM::ContourPathsPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> ContourPathsPFM::defineSettings() const {
    return {
        {"threshold", "Threshold", SettingType::Integer, 127, QVariant(), 0, 255, 0, 255, 1},
        {"epsilon", "Simplify Epsilon", SettingType::Number, 1.5, QVariant(), 0.0, 10.0, 0.0, 10.0, 0.1}
    };
}

QVector<DrawingGeometry> ContourPathsPFM::_process(const cv::Mat& image) {
    int threshold = m_settings["threshold"].toInt();
    double epsilon = m_settings["epsilon"].toDouble();

    cv::Mat img8u;
    image.convertTo(img8u, CV_8UC1);
    
    cv::Mat binary;
    cv::threshold(img8u, binary, threshold, 255, cv::THRESH_BINARY_INV);
    
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(binary, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    
    QVector<DrawingGeometry> geoms;
    for (size_t i = 0; i < contours.size(); ++i) {
        if (isCancelled()) break;
        if (i % 100 == 0) emitProgress(float(i) / contours.size(), geoms.size(), "Contours...");
        
        const auto& cnt = contours[i];
        if (cnt.size() < 2) continue;
        
        std::vector<cv::Point> approx;
        cv::approxPolyDP(cnt, approx, epsilon, true);
        
        Path path;
        for (const auto& p : approx) {
            path.push_back({float(p.x), float(p.y)});
        }
        
        if (path.size() >= 2) {
            DrawingGeometry dg; dg.path = path;
            geoms.push_back(dg);
        }
    }
    return geoms;
}

// -------------------------------------------------------------------------
// AmbientFlowPFM
// -------------------------------------------------------------------------
namespace {
    Path trace_streamline_local(const cv::Mat& fx, const cv::Mat& fy, float x, float y, int max_len, float step_size) {
        Path path;
        int w = fx.cols;
        int h = fx.rows;
        for (int i = 0; i < max_len; ++i) {
            int xi = std::clamp(int(x), 0, w - 1);
            int yi = std::clamp(int(y), 0, h - 1);
            path.push_back({x, y});
            
            float vx = fx.at<float>(yi, xi);
            float vy = fy.at<float>(yi, xi);
            float mag = std::hypot(vx, vy);
            if (mag < 1e-6f) break;
            
            x += (vx / mag) * step_size;
            y += (vy / mag) * step_size;
            if (x < 0 || x >= w || y < 0 || y >= h) break;
        }
        return path;
    }
}

AmbientFlowPFM::AmbientFlowPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> AmbientFlowPFM::defineSettings() const {
    return {
        {"lines", "Lines", SettingType::Integer, 200, QVariant(), 20, 2000, 20, 2000, 10},
        {"length", "Max Length", SettingType::Integer, 150, QVariant(), 20, 500, 20, 500, 10}
    };
}

QVector<DrawingGeometry> AmbientFlowPFM::_process(const cv::Mat& image) {
    int count = m_settings["lines"].toInt();
    int max_len = m_settings["length"].toInt();
    
    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    cv::GaussianBlur(gx, gx, cv::Size(9, 9), 0);
    cv::GaussianBlur(gy, gy, cv::Size(9, 9), 0);
    
    QVector<DrawingGeometry> geoms;
    for (int i = 0; i < count; ++i) {
        if (isCancelled()) break;
        if (i % 20 == 0) emitProgress(float(i) / count, geoms.size(), "Ambient Flow...");
        float x = randUniform(0, image.cols);
        float y = randUniform(0, image.rows);
        Path path = trace_streamline_local(gx, gy, x, y, max_len, 1.2f);
        if (path.size() >= 2) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}

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
