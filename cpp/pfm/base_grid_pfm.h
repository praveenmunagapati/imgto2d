#pragma once
#include "pfm/pfm_base.h"

class BaseGridPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit BaseGridPFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};
