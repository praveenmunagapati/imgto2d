#include "pfm/voronoi_circles_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <map>
#include <queue>
#include <cmath>

QVector<DrawingGeometry> VoronoiCirclesPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    QVector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        if (i % 40 == 0) emitProgress(float(i) / pts.size(), geoms.size(), "Voronoi Circles...");
        float r = nearest_seed_radius(pts[i].x, pts[i].y, pts);
        DrawingGeometry dg; dg.path = generate_circle(pts[i].x, pts[i].y, r, 16);
        geoms.push_back(dg);
    }
    return geoms;
}
