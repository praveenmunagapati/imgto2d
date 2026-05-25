#include "pfm/voronoi_shapes_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <map>
#include <queue>
#include <cmath>

QVector<DrawingGeometry> VoronoiShapesPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    QVector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        float r = nearest_seed_radius(pts[i].x, pts[i].y, pts);
        int sides = 3 + (i % 5);
        DrawingGeometry dg; dg.path = generate_polygon(pts[i].x, pts[i].y, r, sides);
        geoms.push_back(dg);
    }
    return geoms;
}
