#pragma once
#include "pfm/pfm_base.h"

class SketchSpiralsPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchSpiralsPFM(QObject* parent = nullptr);
    QString name()        const override { return "Sketch Spirals"; }
    QString description() const override { return "Draws a continuous spiral, tightening in dark areas."; }
    QString category()    const override { return "Sketch"; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
