#pragma once
#include "pfm/pfm_base.h"

class LayersPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit LayersPFM(QObject* parent = nullptr);
    QString name()     const override { return "Layers PFM"; }
    QString category() const override { return "Composite"; }
    bool isPremium()   const override { return true; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
