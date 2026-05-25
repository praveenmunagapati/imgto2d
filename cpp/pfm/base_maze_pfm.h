#pragma once
#include "pfm/pfm_base.h"

class BaseMazePFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit BaseMazePFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
    virtual std::pair<float, float> transformPoint(float x, float y);
};
