#pragma once
#include "pfm/pfm_base.h"

class SketchQuadBeziersPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchQuadBeziersPFM(QObject* parent = nullptr);
    QString name()        const override { return "Sketch Quad Beziers"; }
    QString description() const override { return "Transforms an image into Quadratic Bezier curves."; }
    QString category()    const override { return "Sketch"; }
    bool    isPremium()   const override { return true; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
