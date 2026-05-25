#pragma once
#include "pfm/sketch_cubic_beziers2_pfm.h"
#include "pfm/pfm_base.h"

class SketchSweepingCurvesPFM : public SketchCubicBeziers2PFM {
    Q_OBJECT
public:
    explicit SketchSweepingCurvesPFM(QObject* parent = nullptr);
    QString name() const override;
};
