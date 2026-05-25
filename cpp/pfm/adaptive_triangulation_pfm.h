#pragma once
#include "pfm/base_adaptive_pfm.h"
#include "pfm/pfm_base.h"

class AdaptiveTriangulationPFM : public BaseAdaptivePFM {
    Q_OBJECT
public:
    explicit AdaptiveTriangulationPFM(QObject* p = nullptr) : BaseAdaptivePFM(p) {}
    QString name() const override { return "Adaptive Triangulation"; }
protected:
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
