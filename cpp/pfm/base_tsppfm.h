#pragma once
#include "pfm/pfm_base.h"

class BaseTSPPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit BaseTSPPFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    virtual std::vector<double> getProbabilities(const cv::Mat& image) = 0;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
