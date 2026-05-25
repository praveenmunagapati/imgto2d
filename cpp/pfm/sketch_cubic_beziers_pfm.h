#pragma once
#include "pfm/pfm_base.h"

class SketchCubicBeziersPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchCubicBeziersPFM(QObject* parent = nullptr);
    QString name()        const override { return "Sketch Cubic Beziers"; }
    QString description() const override { return "Transforms an image into Cubic Bezier curves."; }
    QString category()    const override { return "Sketch"; }
    bool    isPremium()   const override { return true; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
