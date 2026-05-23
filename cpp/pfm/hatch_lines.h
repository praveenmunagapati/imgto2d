#pragma once
#include "pfm/pfm_base.h"

class HatchLinesPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit HatchLinesPFM(QObject* parent = nullptr);
    QString name()        const override { return "Hatch Lines"; }
    QString description() const override { return "Cross-hatching based on image brightness."; }
    QString category()    const override { return "Hatch"; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
