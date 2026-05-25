#include "pfm/streamline_pfms.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

static Path trace_streamline(const cv::Mat& fx, const cv::Mat& fy, float x, float y, int max_len, float step_size) {
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

BaseStreamlinePFM::BaseStreamlinePFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> BaseStreamlinePFM::defineSettings() const {
    return {
        {"line_count", "Line Count", SettingType::Integer, 120, QVariant(), 10, 2000, 10, 2000, 50},
        {"max_length", "Max Length", SettingType::Integer, 200, QVariant(), 20, 2000, 20, 2000, 10},
        {"step_size", "Step Size", SettingType::Number, 1.5, QVariant(), 0.5, 5.0, 0.5, 5.0, 0.1}
    };
}

QVector<DrawingGeometry> StreamlinesEdgeFieldPFM::_process(const cv::Mat& image) {
    int count = m_settings["line_count"].toInt();
    int max_len = m_settings["max_length"].toInt();
    float step = m_settings["step_size"].toDouble();
    
    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    cv::GaussianBlur(gx, gx, cv::Size(7, 7), 0);
    cv::GaussianBlur(gy, gy, cv::Size(7, 7), 0);
    
    // For Edge field, flow is perpendicular to gradient
    cv::Mat fx = -gy;
    cv::Mat fy = gx;
    
    QVector<DrawingGeometry> geoms;
    for (int i = 0; i < count; ++i) {
        if (isCancelled()) break;
        if (i % 20 == 0) emitProgress(float(i) / count, geoms.size(), "Edge Field Streamlines...");
        float x = randUniform(0, image.cols);
        float y = randUniform(0, image.rows);
        Path path = trace_streamline(fx, fy, x, y, max_len, step);
        if (path.size() >= 2) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}

QVector<DrawingGeometry> StreamlinesFlowFieldPFM::_process(const cv::Mat& image) {
    int count = m_settings["line_count"].toInt();
    int max_len = m_settings["max_length"].toInt();
    float step = m_settings["step_size"].toDouble();
    
    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    cv::GaussianBlur(gx, gx, cv::Size(5, 5), 0);
    cv::GaussianBlur(gy, gy, cv::Size(5, 5), 0);
    
    // For Flow field, flow is along gradient
    cv::Mat fx = gx;
    cv::Mat fy = gy;
    
    QVector<DrawingGeometry> geoms;
    for (int i = 0; i < count; ++i) {
        if (isCancelled()) break;
        if (i % 20 == 0) emitProgress(float(i) / count, geoms.size(), "Flow Field Streamlines...");
        float x = randUniform(0, image.cols);
        float y = randUniform(0, image.rows);
        Path path = trace_streamline(fx, fy, x, y, max_len, step);
        if (path.size() >= 2) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}

QVector<PFMSetting> StreamlinesSuperformulaPFM::defineSettings() const {
    auto s = BaseStreamlinePFM::defineSettings();
    s.push_back({"wave_amp", "Wave Amplitude", SettingType::Number, 2.0, QVariant(), 0.0, 20.0, 0.0, 20.0, 0.5});
    return s;
}

QVector<DrawingGeometry> StreamlinesSuperformulaPFM::_process(const cv::Mat& image) {
    int count = m_settings["line_count"].toInt();
    int max_len = m_settings["max_length"].toInt();
    float step = m_settings["step_size"].toDouble();
    float amp = m_settings["wave_amp"].toDouble();
    
    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    cv::GaussianBlur(gx, gx, cv::Size(3, 3), 0);
    cv::GaussianBlur(gy, gy, cv::Size(3, 3), 0);
    
    cv::Mat fx = gx;
    cv::Mat fy = gy;
    
    QVector<DrawingGeometry> geoms;
    for (int i = 0; i < count; ++i) {
        if (isCancelled()) break;
        if (i % 20 == 0) emitProgress(float(i) / count, geoms.size(), "Superformula Streamlines...");
        float x = randUniform(0, image.cols);
        float y = randUniform(0, image.rows);
        Path path = trace_streamline(fx, fy, x, y, max_len, step);
        if (path.size() >= 2 && amp > 0) {
            Path waved;
            for (size_t j = 0; j < path.size(); ++j) {
                float wx = path[j].first + std::sin(j * 0.2f) * amp;
                float wy = path[j].second + std::cos(j * 0.15f) * amp;
                waved.push_back({wx, wy});
            }
            path = waved;
        }
        if (path.size() >= 2) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}
