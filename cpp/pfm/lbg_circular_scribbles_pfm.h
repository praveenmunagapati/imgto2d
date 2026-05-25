#pragma once
#include "pfm/adaptive_circular_scribbles_pfm.h"
#include "pfm/adaptive_pfm.h"

class LBGCircularScribblesPFM : public AdaptiveCircularScribblesPFM {
    Q_OBJECT
public:
    explicit LBGCircularScribblesPFM(QObject* p = nullptr) : AdaptiveCircularScribblesPFM(p) {}
    QString name() const override { return "LBG Circular Scribbles"; }
    QString category() const override { return "LBG"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};
