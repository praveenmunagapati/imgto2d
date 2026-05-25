#pragma once
#include "pfm/adaptive_pfm.h"

class LBGStipplingPFM : public AdaptiveStipplingPFM {
    Q_OBJECT
public:
    explicit LBGStipplingPFM(QObject* p = nullptr) : AdaptiveStipplingPFM(p) {}
    QString name() const override { return "LBG Stippling"; }
    QString category() const override { return "LBG"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};
