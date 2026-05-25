#pragma once
#include "pfm/pfm_base.h"

class LabyrinthClassicPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit LabyrinthClassicPFM(QObject* parent = nullptr);
    QString name() const override { return "Labyrinth Classic"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
