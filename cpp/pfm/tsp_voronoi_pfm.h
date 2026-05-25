#pragma once
#include "pfm/base_tsppfm.h"
#include "pfm/pfm_base.h"

class TSPVoronoiPFM : public BaseTSPPFM {
    Q_OBJECT
public:
    explicit TSPVoronoiPFM(QObject* parent = nullptr);
    QString name() const override { return "TSP Voronoi"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    std::vector<double> getProbabilities(const cv::Mat& image) override;
};
