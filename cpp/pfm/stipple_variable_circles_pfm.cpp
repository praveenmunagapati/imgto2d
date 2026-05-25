#include "pfm/stipple_variable_circles_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

Path StippleVariableCirclesPFM::generateShape(float cx, float cy, float r) {
    return generate_circle(cx, cy, r * (0.5 + randUniform(0, 1)), 8);
}
