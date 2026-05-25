#include "pfm/stipple_squares_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

Path StippleSquaresPFM::generateShape(float cx, float cy, float r) { return generate_polygon(cx, cy, r, 4, 45); }
