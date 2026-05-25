#pragma once
#include "pfm/pfm_base.h"

class BaseStipplePFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit BaseStipplePFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    virtual Path generateShape(float cx, float cy, float r) = 0;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
