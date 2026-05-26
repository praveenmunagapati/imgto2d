#pragma once
#include "pfm/base_multi_hatch_pfm.h"
#include "pfm/pfm_base.h"

class HatchCrossPFM : public BaseMultiHatchPFM {
    Q_OBJECT
public: explicit HatchCrossPFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch Cross";} protected: std::vector<float> getAngles() const override { return {45.0f, -45.0f}; } };
