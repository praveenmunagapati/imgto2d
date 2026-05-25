#pragma once
#include "pfm/pfm_base.h"

class StippleLayersPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit StippleLayersPFM(QObject* parent = nullptr);
    QString name()     const override { return "Stipple Layers"; }
    QString category() const override { return "Composite"; }
    bool isPremium()   const override { return true; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
