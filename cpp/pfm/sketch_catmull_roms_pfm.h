#pragma once
#include "pfm/pfm_base.h"

class SketchCatmullRomsPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchCatmullRomsPFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Catmull-Roms"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
