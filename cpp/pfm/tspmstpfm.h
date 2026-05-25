#pragma once
#include "pfm/pfm_base.h"

class TSPMSTPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit TSPMSTPFM(QObject* parent = nullptr);
    QString name() const override { return "Minimum Spanning Tree"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
