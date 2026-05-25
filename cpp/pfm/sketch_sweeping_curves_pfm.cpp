#include "pfm/sketch_sweeping_curves_pfm.h"
#include "pfm/sketch_curves.h"
#include "pfm/sketch_lines.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

// -------------------------------------------------------------------------
SketchSweepingCurvesPFM::SketchSweepingCurvesPFM(QObject* parent) : SketchCubicBeziers2PFM(parent) {}

QString SketchSweepingCurvesPFM::name() const { return "Sketch Sweeping Curves"; }
