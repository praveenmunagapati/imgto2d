#pragma once
#include "pfm/base_adaptive_pfm.h"
#include "pfm/pfm_base.h"

class AdaptiveTreePFM : public BaseAdaptivePFM {
    Q_OBJECT
public:
    explicit AdaptiveTreePFM(QObject* p = nullptr) : BaseAdaptivePFM(p) {}
    QString name() const override { return "Adaptive Tree"; }
protected:
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
