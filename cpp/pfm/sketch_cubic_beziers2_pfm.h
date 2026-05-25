#pragma once
#include "pfm/pfm_base.h"

class SketchCubicBeziers2PFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchCubicBeziers2PFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Cubic Beziers 2"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
