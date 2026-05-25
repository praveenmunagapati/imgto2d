#include "pfm/stipple_circles_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

Path StippleCirclesPFM::generateShape(float cx, float cy, float r) { return generate_circle(cx, cy, r, 12); }
