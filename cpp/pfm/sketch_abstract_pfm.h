#pragma once
#include "pfm/pfm_base.h"

class SketchAbstractPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchAbstractPFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Abstract"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
