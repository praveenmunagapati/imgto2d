#include "pfm/stipple_triangles_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

Path StippleTrianglesPFM::generateShape(float cx, float cy, float r) { return generate_polygon(cx, cy, r, 3, 30); }
