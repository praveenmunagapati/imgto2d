#include "pfm/maze_tri_pfm.h"
#include <opencv2/imgproc.hpp>
#include <map>
#include <queue>
#include <cmath>

std::pair<float, float> MazeTriPFM::transformPoint(float x, float y) {
    return {(x - y) * std::cos(M_PI / 6.0f), (x + y) * std::sin(M_PI / 6.0f)};
}
