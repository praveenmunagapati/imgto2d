#pragma once
#include "pfm/pfm_base.h"

class BaseMultiHatchPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit BaseMultiHatchPFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
    virtual std::vector<float> getAngles() const = 0;
};
