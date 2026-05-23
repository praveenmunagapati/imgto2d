#pragma once
#include "pfm/pfm_base.h"

class SketchCurvesPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchCurvesPFM(QObject* parent = nullptr);
    QString name()        const override { return "Sketch Curves"; }
    QString description() const override { return "Transforms an image into smooth curves using brightness data."; }
    QString category()    const override { return "Sketch"; }
    bool    isPremium()   const override { return true; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

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
