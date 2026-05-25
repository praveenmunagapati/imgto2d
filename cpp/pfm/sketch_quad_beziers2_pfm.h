#pragma once
#include "pfm/sketch_cubic_beziers2_pfm.h"
#include "pfm/pfm_base.h"

class SketchQuadBeziers2PFM : public SketchCubicBeziers2PFM {
    Q_OBJECT
public:
    explicit SketchQuadBeziers2PFM(QObject* parent = nullptr) : SketchCubicBeziers2PFM(parent) {}
    QString name() const override { return "Sketch Quad Beziers 2"; }
protected:
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
