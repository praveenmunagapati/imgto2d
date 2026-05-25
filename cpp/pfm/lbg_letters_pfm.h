#pragma once
#include "pfm/_letters_base_pfm.h"
#include "pfm/pfm_base.h"

class LBGLettersPFM : public _LettersBasePFM {
    Q_OBJECT
public:
    explicit LBGLettersPFM(QObject* parent = nullptr);
    QString name() const override { return "LBG Letters"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    int getLloydIters() const override;
};
