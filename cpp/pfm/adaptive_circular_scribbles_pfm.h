#pragma once
#include "pfm/base_adaptive_pfm.h"
#include "pfm/pfm_base.h"

class AdaptiveCircularScribblesPFM : public BaseAdaptivePFM {
    Q_OBJECT
public:
    explicit AdaptiveCircularScribblesPFM(QObject* p = nullptr);
    QString name() const override { return "Adaptive Circular Scribbles"; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
