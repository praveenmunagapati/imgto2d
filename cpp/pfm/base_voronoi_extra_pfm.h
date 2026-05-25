#pragma once
#include "pfm/pfm_base.h"

class BaseVoronoiExtraPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit BaseVoronoiExtraPFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    std::vector<cv::Point2f> getSeeds(const cv::Mat& image);
};
