#pragma once
#include "pfm/pfm_base.h"

class SketchShapesPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchShapesPFM(QObject* parent = nullptr);
    QString name()        const override { return "Sketch Shapes"; }
    QString description() const override { return "Transforms an image into assorted shapes using brightness data."; }
    QString category()    const override { return "Sketch"; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
