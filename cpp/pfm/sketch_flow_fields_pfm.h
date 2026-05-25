#pragma once
#include "pfm/pfm_base.h"

class SketchFlowFieldsPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchFlowFieldsPFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Flow Fields"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
