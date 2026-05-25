#include "pfm/stipple_stars_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

Path StippleStarsPFM::generateShape(float cx, float cy, float r) { 
    Path p1 = generate_polygon(cx, cy, r, 5, 0);
    Path p2 = generate_polygon(cx, cy, r*0.5f, 5, 36);
    p1.insert(p1.end(), p2.begin(), p2.end());
    return p1;
}
