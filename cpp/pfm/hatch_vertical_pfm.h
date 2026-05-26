#pragma once
#include "pfm/base_multi_hatch_pfm.h"
#include "pfm/pfm_base.h"

class HatchVerticalPFM : public BaseMultiHatchPFM {
    Q_OBJECT
public: explicit HatchVerticalPFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch Vertical";} protected: std::vector<float> getAngles() const override { return {90.0f}; } };
