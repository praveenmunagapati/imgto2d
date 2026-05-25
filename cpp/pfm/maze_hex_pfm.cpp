#include "pfm/maze_hex_pfm.h"
#include <opencv2/imgproc.hpp>
#include <map>
#include <queue>
#include <cmath>

std::pair<float, float> MazeHexPFM::transformPoint(float x, float y) {
    float offset = (int(y / 10.0) % 2 != 0) ? 5.0f : 0.0f;
    return {x + offset, y * std::sqrt(3.0f) / 2.0f};
}
