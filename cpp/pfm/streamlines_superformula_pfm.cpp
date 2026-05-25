#include "pfm/streamlines_superformula_pfm.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

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
