#pragma once
#include "pfm/_letters_base_pfm.h"
#include "pfm/pfm_base.h"

class AdaptiveLettersPFM : public _LettersBasePFM {
    Q_OBJECT
public:
    explicit AdaptiveLettersPFM(QObject* parent = nullptr) : _LettersBasePFM(parent) {}
    QString name() const override { return "Adaptive Letters"; }
protected:
    int getLloydIters() const override { return 0; }
};
