#include "pfm/voronoi_triangulation_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <map>
#include <queue>
#include <cmath>

QVector<DrawingGeometry> VoronoiTriangulationPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    emitProgress(0.5, 0, "Delaunay...");
    cv::Subdiv2D subdiv(cv::Rect(0, 0, image.cols, image.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<cv::Vec4f> edges;
    subdiv.getEdgeList(edges);
    QVector<DrawingGeometry> geoms;
    for (const auto& e : edges) {
        DrawingGeometry dg; dg.path = {{e[0], e[1]}, {e[2], e[3]}};
        geoms.push_back(dg);
    }
    return geoms;
}
