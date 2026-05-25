#include "pfm/tsp_voronoi_pfm.h"
#include <opencv2/imgproc.hpp>
#include <map>
#include <queue>
#include <cmath>

// -------------------------------------------------------------------------
TSPVoronoiPFM::TSPVoronoiPFM(QObject* parent) : BaseTSPPFM(parent) {}

QVector<PFMSetting> TSPVoronoiPFM::defineSettings() const {
    auto base = BaseTSPPFM::defineSettings();
    base.append({"voronoi_thresh", "Edge Threshold", SettingType::Integer, 80, QVariant(), 10, 255, 10, 255, 10});
    return base;
}

std::vector<double> TSPVoronoiPFM::getProbabilities(const cv::Mat& image) {
    int thresh = m_settings.contains("voronoi_thresh") ?
                 m_settings["voronoi_thresh"].toInt() : 80;
    cv::Mat edges;
    cv::Canny(image, edges, thresh, thresh * 2);
    std::vector<double> probs(image.cols * image.rows, 0.0);
    for (int y = 0; y < image.rows; ++y) {
        const uchar* row = edges.ptr<uchar>(y);
        for (int x = 0; x < image.cols; ++x) {
            if (row[x] > 0) probs[y * image.cols + x] = row[x];
        }
    }
    return probs;
}
