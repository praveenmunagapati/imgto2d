#include "pfm/adaptive_triangulation_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <cmath>
#include <map>
#include <queue>

// ---------------------------------------------------------------------------

QVector<DrawingGeometry> AdaptiveTriangulationPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image;
    }
    auto pts = getSeeds(workImg);
    if (pts.empty()) return {};
    emitProgress(0.5f, 0, "Delaunay triangulation...");
    cv::Subdiv2D subdiv(cv::Rect(0, 0, workImg.cols, workImg.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<cv::Vec4f> edges;
    subdiv.getEdgeList(edges);
    QVector<DrawingGeometry> geoms;
    for (const auto& e : edges) {
        geoms.push_back(DrawingGeometry{{{e[0], e[1]}, {e[2], e[3]}}, 0});
    }
    emitProgress(1.0f, (int)geoms.size(), "Triangulation done");
    return geoms;
}
