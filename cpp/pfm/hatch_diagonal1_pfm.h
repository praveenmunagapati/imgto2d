#pragma once
#include "pfm/base_multi_hatch_pfm.h"
#include "pfm/pfm_base.h"

class HatchDiagonal1PFM : public BaseMultiHatchPFM {
    Q_OBJECT
public: explicit HatchDiagonal1PFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch Diagonal 1";} protected: std::vector<float> getAngles() const override { return {45.0f}; } };
