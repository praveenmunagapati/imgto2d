#pragma once
#include "pfm/base_tsppfm.h"
#include "pfm/pfm_base.h"

class TSPClassicPFM : public BaseTSPPFM {
    Q_OBJECT
public:
    explicit TSPClassicPFM(QObject* parent = nullptr) : BaseTSPPFM(parent) {}
    QString name() const override { return "TSP Classic"; }
protected:
    std::vector<double> getProbabilities(const cv::Mat& image) override;
};
