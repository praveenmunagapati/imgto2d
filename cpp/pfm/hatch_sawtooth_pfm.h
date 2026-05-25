#pragma once
#include "pfm/pfm_base.h"

class HatchSawtoothPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit HatchSawtoothPFM(QObject* parent = nullptr);
    QString name() const override { return "Hatch Sawtooth"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
