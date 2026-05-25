#include "pfm/stipple_hexagons_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

Path StippleHexagonsPFM::generateShape(float cx, float cy, float r) { return generate_polygon(cx, cy, r, 6, 0); }
