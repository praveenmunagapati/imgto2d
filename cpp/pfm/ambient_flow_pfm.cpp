#include "pfm/ambient_flow_pfm.h"
#include <opencv2/imgproc.hpp>
#include <cmath>
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
