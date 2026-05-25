#pragma once
#include "pfm/base_adaptive_pfm.h"
#include "pfm/pfm_base.h"

class AdaptiveShapesPFM : public BaseAdaptivePFM {
    Q_OBJECT
public:
    explicit AdaptiveShapesPFM(QObject* p = nullptr);
    QString name() const override { return "Adaptive Shapes"; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
