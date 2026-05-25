#pragma once
#include "pfm/base_adaptive_pfm.h"
#include "pfm/pfm_base.h"

class AdaptiveDashesPFM : public BaseAdaptivePFM {
    Q_OBJECT
public:
    explicit AdaptiveDashesPFM(QObject* p = nullptr) : BaseAdaptivePFM(p) {}
    QString name() const override { return "Adaptive Dashes"; }
protected:
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
