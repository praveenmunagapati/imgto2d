#include "pfm/stipple_crosses_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

Path StippleCrossesPFM::generateShape(float cx, float cy, float r) {
    return {{cx-r, cy}, {cx+r, cy}, {cx, cy}, {cx, cy-r}, {cx, cy+r}};
}
