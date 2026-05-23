#pragma once
#include "pfm/pfm_base.h"

class SketchSquaresPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchSquaresPFM(QObject* parent = nullptr);
    QString name()        const override { return "Sketch Squares"; }
    QString description() const override { return "Transforms an image into squares using brightness data."; }
    QString category()    const override { return "Sketch"; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
