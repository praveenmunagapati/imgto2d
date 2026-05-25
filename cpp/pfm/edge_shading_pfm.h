#pragma once
#include "pfm/pfm_base.h"

class EdgeShadingPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit EdgeShadingPFM(QObject* parent = nullptr);
    QString name() const override { return "Edge Shading"; }
    QString category() const override { return "Special"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
