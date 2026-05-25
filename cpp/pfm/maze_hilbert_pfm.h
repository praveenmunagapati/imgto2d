#pragma once
#include "pfm/pfm_base.h"

class MazeHilbertPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit MazeHilbertPFM(QObject* parent = nullptr);
    QString name() const override { return "Hilbert Curve"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
