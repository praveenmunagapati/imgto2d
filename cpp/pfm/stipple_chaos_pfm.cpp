#include "pfm/stipple_chaos_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

Path StippleChaosPFM::generateShape(float cx, float cy, float r) {
    int pts = int(randUniform(0, 4)) + 3;
    return generate_polygon(cx, cy, r, pts, randUniform(0, 360));
}
