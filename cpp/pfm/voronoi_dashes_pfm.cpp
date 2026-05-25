#include "pfm/voronoi_dashes_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <map>
#include <queue>
#include <cmath>

QVector<DrawingGeometry> VoronoiDashesPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    cv::Subdiv2D subdiv(cv::Rect(0, 0, image.cols, image.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<std::vector<cv::Point2f>> facets;
    std::vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);
    QVector<DrawingGeometry> geoms;
    for (const auto& facet : facets) {
        if (facet.size() < 2) continue;
        for (size_t i = 0; i < facet.size() - 1; ++i) {
            float x0 = facet[i].x, y0 = facet[i].y;
            float x1 = facet[i+1].x, y1 = facet[i+1].y;
            float mx = (x0 + x1) / 2.0f;
            float my = (y0 + y1) / 2.0f;
            float length = std::hypot(x1 - x0, y1 - y0) * 0.4f;
            float angle = std::atan2(y1 - y0, x1 - x0);
            DrawingGeometry dg;
            dg.path = {
                {mx - std::cos(angle) * length / 2.0f, my - std::sin(angle) * length / 2.0f},
                {mx + std::cos(angle) * length / 2.0f, my + std::sin(angle) * length / 2.0f}
            };
            geoms.push_back(dg);
        }
    }
    return geoms;
}
