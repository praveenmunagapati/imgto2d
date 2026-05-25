#pragma once
#include "pfm/pfm_base.h"

class SketchVoronoiPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchVoronoiPFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Voronoi"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
