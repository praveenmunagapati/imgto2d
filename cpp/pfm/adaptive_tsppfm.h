#pragma once
#include "pfm/base_adaptive_pfm.h"
#include "pfm/pfm_base.h"

class AdaptiveTSPPFM : public BaseAdaptivePFM {
    Q_OBJECT
public:
    explicit AdaptiveTSPPFM(QObject* p = nullptr) : BaseAdaptivePFM(p) {}
    QString name() const override { return "Adaptive TSP"; }
protected:
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
