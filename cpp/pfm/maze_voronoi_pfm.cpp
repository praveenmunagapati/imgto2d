#include "pfm/maze_voronoi_pfm.h"
#include <opencv2/imgproc.hpp>
#include <map>
#include <queue>
#include <cmath>

std::pair<float, float> MazeVoronoiPFM::transformPoint(float x, float y) {
    return {x + std::sin(y / 10.0f) * 5.0f, y + std::cos(x / 10.0f) * 5.0f};
}
