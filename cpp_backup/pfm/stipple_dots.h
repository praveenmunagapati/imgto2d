#pragma once
#include "pfm/pfm_base.h"

class StippleDotsPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit StippleDotsPFM(QObject* parent = nullptr);
    QString name()        const override { return "Stipple Dots"; }
    QString description() const override { return "Pointillism based on image brightness."; }
    QString category()    const override { return "Stipple"; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
