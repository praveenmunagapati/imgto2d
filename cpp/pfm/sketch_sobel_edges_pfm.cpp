#include "pfm/sketch_sobel_edges_pfm.h"
#include "pfm/sketch_curves.h"
#include "pfm/sketch_lines.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

SketchSobelEdgesPFM::SketchSobelEdgesPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> SketchSobelEdgesPFM::defineSettings() const {
    return SketchLinesPFM().settingsList();
}

QVector<DrawingGeometry> SketchSobelEdgesPFM::_process(const cv::Mat& image) {
    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    cv::Mat mag;
    cv::magnitude(gx, gy, mag);
    double maxVal;
    cv::minMaxLoc(mag, nullptr, &maxVal);
    cv::Mat edge_img;
    mag.convertTo(edge_img, CV_32F, 255.0 / (maxVal + 1e-6));
    edge_img = 255.0 - edge_img;

    SketchLinesPFM sketch;
    for (auto it = m_settings.constBegin(); it != m_settings.constEnd(); ++it) {
        sketch.set(it.key(), it.value().currentValue());
    }
    connect(&sketch, &PathFindingModule::progressUpdate, this, &PathFindingModule::progressUpdate);
    return sketch.process(edge_img);
}
