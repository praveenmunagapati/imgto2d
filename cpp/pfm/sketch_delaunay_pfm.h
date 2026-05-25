#pragma once
#include "pfm/pfm_base.h"

class SketchDelaunayPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchDelaunayPFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Delaunay"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
