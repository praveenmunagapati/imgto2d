#include "pfm/sketch_voronoi_pfm.h"
#include "pfm/sketch_curves.h"
#include "pfm/sketch_lines.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

// -------------------------------------------------------------------------
// SketchVoronoiPFM
// -------------------------------------------------------------------------
SketchVoronoiPFM::SketchVoronoiPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> SketchVoronoiPFM::defineSettings() const {
    return {
        {"nodes", "Nodes", SettingType::Integer, 500, QVariant(), 10, 5000, 10, 5000, 10}
    };
}

QVector<DrawingGeometry> SketchVoronoiPFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int nodes = m_settings["nodes"].toInt();

    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    std::vector<cv::Point2f> pts;
    for (int i = 0; i < nodes; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        pts.push_back(cv::Point2f(idx % w, idx / w));
    }

    cv::Subdiv2D subdiv(cv::Rect(0, 0, w, h));
    for (const auto& p : pts) subdiv.insert(p);

    std::vector<std::vector<cv::Point2f>> facets;
    std::vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);

    QVector<DrawingGeometry> geoms;
    for (const auto& facet : facets) {
        if (facet.size() < 2) continue;
        for (size_t i = 0; i < facet.size() - 1; ++i) {
            DrawingGeometry dg;
            dg.path = {{facet[i].x, facet[i].y}, {facet[i+1].x, facet[i+1].y}};
            geoms.push_back(dg);
        }
    }
    return geoms;
}
