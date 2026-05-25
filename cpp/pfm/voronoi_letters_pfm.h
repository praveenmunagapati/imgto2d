#pragma once
#include "pfm/_letters_base_pfm.h"
#include "pfm/pfm_base.h"

class VoronoiLettersPFM : public _LettersBasePFM {
    Q_OBJECT
public:
    explicit VoronoiLettersPFM(QObject* parent = nullptr) : _LettersBasePFM(parent) {}
    QString name() const override { return "Voronoi Letters"; }
protected:
    int getLloydIters() const override;
};
