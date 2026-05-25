#pragma once
#include "pfm/pfm_base.h"

class BaseAdaptivePFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit BaseAdaptivePFM(QObject* parent = nullptr);
    QString category() const override { return "Adaptive"; }
    bool    isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    // Returns brightness-weighted, Lloyd-relaxed seed points
    std::vector<cv::Point2f> getSeeds(const cv::Mat& image);
};
