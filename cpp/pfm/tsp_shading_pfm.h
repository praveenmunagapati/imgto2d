#pragma once
#include "pfm/base_tsppfm.h"
#include "pfm/pfm_base.h"

class TSPShadingPFM : public BaseTSPPFM {
    Q_OBJECT
public:
    explicit TSPShadingPFM(QObject* parent = nullptr) : BaseTSPPFM(parent) {}
    QString name() const override { return "TSP Shading"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    std::vector<double> getProbabilities(const cv::Mat& image) override;
};
