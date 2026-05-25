#pragma once
#include "pfm/adaptive_tsppfm.h"
#include "pfm/adaptive_pfm.h"

class LBGTSPPFM : public AdaptiveTSPPFM {
    Q_OBJECT
public:
    explicit LBGTSPPFM(QObject* p = nullptr) : AdaptiveTSPPFM(p) {}
    QString name() const override { return "LBG TSP"; }
    QString category() const override { return "LBG"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};
