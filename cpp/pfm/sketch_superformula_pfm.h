#pragma once
#include "pfm/pfm_base.h"

class SketchSuperformulaPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchSuperformulaPFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Superformula"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
