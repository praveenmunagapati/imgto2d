#pragma once
#include "pfm/pfm_base.h"

class SketchSobelEdgesPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchSobelEdgesPFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Sobel Edges"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
