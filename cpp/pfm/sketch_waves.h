#pragma once
#include "pfm/pfm_base.h"

class SketchWavesPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchWavesPFM(QObject* parent = nullptr);
    QString name()        const override { return "Sketch Waves"; }
    QString description() const override { return "Draws sine waves varying amplitude by brightness."; }
    QString category()    const override { return "Sketch"; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
