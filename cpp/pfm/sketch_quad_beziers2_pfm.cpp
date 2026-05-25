#include "pfm/sketch_quad_beziers2_pfm.h"
#include "pfm/sketch_curves.h"
#include "pfm/sketch_lines.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

// -------------------------------------------------------------------------
// Phase 9: Remaining Sketch Variants
// -------------------------------------------------------------------------

QVector<DrawingGeometry> SketchQuadBeziers2PFM::_process(const cv::Mat& image) {
    return SketchCubicBeziers2PFM::_process(image);
}
