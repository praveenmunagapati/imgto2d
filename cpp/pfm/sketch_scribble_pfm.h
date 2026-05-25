#pragma once
#include "pfm/pfm_base.h"

class SketchScribblePFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchScribblePFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Scribble"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
