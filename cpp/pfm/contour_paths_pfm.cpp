#include "pfm/contour_paths_pfm.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

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
