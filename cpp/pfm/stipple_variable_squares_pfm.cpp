#include "pfm/stipple_variable_squares_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

Path StippleVariableSquaresPFM::generateShape(float cx, float cy, float r) {
    return generate_polygon(cx, cy, r * (0.5 + randUniform(0, 1)), 4, randUniform(0, 90));
}
