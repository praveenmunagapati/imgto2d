#pragma once
#include "pfm/base_multi_hatch_pfm.h"
#include "pfm/pfm_base.h"

class HatchGridPFM : public BaseMultiHatchPFM {
    Q_OBJECT
public: explicit HatchGridPFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch Grid";} protected: std::vector<float> getAngles() const override { return {0.0f, 90.0f}; } };
