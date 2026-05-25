#include "pfm/maze_circle_pfm.h"
#include <opencv2/imgproc.hpp>
#include <map>
#include <queue>
#include <cmath>

std::pair<float, float> MazeCirclePFM::transformPoint(float x, float y) {
    float r = y + 10.0f;
    float theta = x / 10.0f;
    return {r * std::cos(theta), r * std::sin(theta)};
}
