#pragma once
#include "pfm/adaptive_dashes_pfm.h"
#include "pfm/adaptive_pfm.h"

class LBGDashesPFM : public AdaptiveDashesPFM {
    Q_OBJECT
public:
    explicit LBGDashesPFM(QObject* p = nullptr) : AdaptiveDashesPFM(p) {}
    QString name() const override { return "LBG Dashes"; }
    QString category() const override { return "LBG"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};
