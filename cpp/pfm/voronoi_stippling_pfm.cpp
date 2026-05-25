#include "pfm/voronoi_stippling_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <map>
#include <queue>
#include <cmath>

QVector<DrawingGeometry> VoronoiStipplingPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    QVector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        int xi = std::clamp(int(pts[i].x), 0, image.cols - 1);
        int yi = std::clamp(int(pts[i].y), 0, image.rows - 1);
        float dark = (255.0f - image.at<float>(yi, xi)) / 255.0f;
        float r = std::max(0.4f, nearest_seed_radius(pts[i].x, pts[i].y, pts) * 0.15f * dark);
        DrawingGeometry dg; dg.path = generate_circle(pts[i].x, pts[i].y, r, 8);
        geoms.push_back(dg);
    }
    return geoms;
}
