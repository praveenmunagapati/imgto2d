#pragma once
#include "pfm/base_tsppfm.h"
#include "pfm/pfm_base.h"

class TSPStipplePFM : public BaseTSPPFM {
    Q_OBJECT
public:
    explicit TSPStipplePFM(QObject* parent = nullptr) : BaseTSPPFM(parent) {}
    QString name() const override { return "TSP Stipple"; }
protected:
    std::vector<double> getProbabilities(const cv::Mat& image) override;
};
