#pragma once
#include "pfm/adaptive_shapes_pfm.h"
#include "pfm/adaptive_pfm.h"

class LBGShapesPFM : public AdaptiveShapesPFM {
    Q_OBJECT
public:
    explicit LBGShapesPFM(QObject* p = nullptr) : AdaptiveShapesPFM(p) {}
    QString name() const override { return "LBG Shapes"; }
    QString category() const override { return "LBG"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};
