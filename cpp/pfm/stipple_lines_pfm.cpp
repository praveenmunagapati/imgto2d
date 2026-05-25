#include "pfm/stipple_lines_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

Path StippleLinesPFM::generateShape(float cx, float cy, float r) {
    return {{cx-r, cy-r}, {cx+r, cy+r}};
}
