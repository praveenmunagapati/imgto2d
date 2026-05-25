#pragma once
#include "pfm/base_multi_hatch_pfm.h"
#include "pfm/pfm_base.h"

class HatchHorizontalPFM : public BaseMultiHatchPFM { Q_OBJECT public: explicit HatchHorizontalPFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch Horizontal";} protected: std::vector<float> getAngles() const override { return {0.0f}; } };
