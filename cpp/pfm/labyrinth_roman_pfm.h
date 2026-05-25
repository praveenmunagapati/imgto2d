#pragma once
#include "pfm/labyrinth_classic_pfm.h"
#include "pfm/pfm_base.h"

class LabyrinthRomanPFM : public LabyrinthClassicPFM {
    Q_OBJECT
public:
    explicit LabyrinthRomanPFM(QObject* p = nullptr) : LabyrinthClassicPFM(p) {}
    QString name() const override { return "Labyrinth Roman"; }
};
