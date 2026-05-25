#include "pfm/mosaic_voronoi_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>

QVector<PFMSetting> MosaicVoronoiPFM::defineSettings() const {
    return {
        {"cell_count", "Cell Count", SettingType::Integer, 200, QVariant(), 20, 2000, 20, 2000, 50}
    };
}

QVector<DrawingGeometry> MosaicVoronoiPFM::_process(const cv::Mat& image) {
    int cell_count = m_settings["cell_count"].toInt();
    std::vector<double> probs(image.cols * image.rows, 0.0);
    double sum = 0.0;
    for (int y = 0; y < image.rows; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < image.cols; ++x) {
            double d = 255.0 - row[x];
            if (d > 0) { probs[y * image.cols + x] = d; sum += d; }
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    std::vector<cv::Point2f> pts;
    for (int i = 0; i < cell_count; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        pts.push_back(cv::Point2f(idx % image.cols, idx / image.cols));
    }

    emitProgress(0.5f, 0, "Mosaic Voronoi...");
    cv::Subdiv2D subdiv(cv::Rect(0, 0, image.cols, image.rows));
    for (const auto& p : pts) subdiv.insert(p);

    std::vector<std::vector<cv::Point2f>> facets;
    std::vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);
    QVector<DrawingGeometry> geoms;
    for (const auto& facet : facets) {
        if (facet.size() >= 2) {
            DrawingGeometry dg;
            for (const auto& pt : facet) dg.path.push_back({pt.x, pt.y});
            dg.path.push_back({facet[0].x, facet[0].y});
            geoms.push_back(dg);
        }
    }
    return geoms;
}
