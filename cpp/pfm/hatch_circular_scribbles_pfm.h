#pragma once
#include "pfm/pfm_base.h"

class HatchCircularScribblesPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit HatchCircularScribblesPFM(QObject* parent = nullptr);
    QString name() const override { return "Hatch Circular Scribbles"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
