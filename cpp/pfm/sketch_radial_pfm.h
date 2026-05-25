#pragma once
#include "pfm/pfm_base.h"

class SketchRadialPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchRadialPFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Radial"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
