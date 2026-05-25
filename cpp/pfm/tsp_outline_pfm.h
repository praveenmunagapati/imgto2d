#pragma once
#include "pfm/base_tsppfm.h"
#include "pfm/pfm_base.h"

class TSPOutlinePFM : public BaseTSPPFM {
    Q_OBJECT
public:
    explicit TSPOutlinePFM(QObject* parent = nullptr) : BaseTSPPFM(parent) {}
    QString name() const override { return "TSP Outline"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    std::vector<double> getProbabilities(const cv::Mat& image) override;
};
