#pragma once
#include "pfm/adaptive_tree_pfm.h"
#include "pfm/adaptive_pfm.h"

class LBGTreePFM : public AdaptiveTreePFM {
    Q_OBJECT
public:
    explicit LBGTreePFM(QObject* p = nullptr) : AdaptiveTreePFM(p) {}
    QString name() const override { return "LBG Tree"; }
    QString category() const override { return "LBG"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};
