#pragma once
#include "pfm/pfm_base.h"

class BaseStreamlinePFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit BaseStreamlinePFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};
