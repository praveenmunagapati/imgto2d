#pragma once
#include "pfm/pfm_base.h"

class MazePeanoPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit MazePeanoPFM(QObject* parent = nullptr);
    QString name() const override { return "Peano Curve"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
