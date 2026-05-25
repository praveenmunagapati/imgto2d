#pragma once
#include "pfm/pfm_base.h"

class BaseMosaicPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit BaseMosaicPFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};
