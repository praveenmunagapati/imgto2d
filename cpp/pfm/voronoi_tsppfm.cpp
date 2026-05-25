#include "pfm/voronoi_tsppfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <map>
#include <queue>
#include <cmath>

QVector<DrawingGeometry> VoronoiTSPPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    if (pts.size() < 2) return {};
    Path path = solve_tsp_nn(pts, [this](){ return isCancelled(); });
    DrawingGeometry dg; dg.path = path;
    return {dg};
}
