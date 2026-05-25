#include "pfm/voronoi_diagram_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <map>
#include <queue>
#include <cmath>

QVector<DrawingGeometry> VoronoiDiagramPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
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
