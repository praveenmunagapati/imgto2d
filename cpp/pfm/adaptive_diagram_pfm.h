#pragma once
#include "pfm/base_adaptive_pfm.h"
#include "pfm/pfm_base.h"

class AdaptiveDiagramPFM : public BaseAdaptivePFM {
    Q_OBJECT
public:
    explicit AdaptiveDiagramPFM(QObject* p = nullptr) : BaseAdaptivePFM(p) {}
    QString name() const override { return "Adaptive Diagram"; }
protected:
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
