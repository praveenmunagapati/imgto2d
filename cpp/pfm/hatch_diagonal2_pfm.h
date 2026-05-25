#pragma once
#include "pfm/base_multi_hatch_pfm.h"
#include "pfm/pfm_base.h"

class HatchDiagonal2PFM : public BaseMultiHatchPFM { Q_OBJECT public: explicit HatchDiagonal2PFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch Diagonal 2";} protected: std::vector<float> getAngles() const override { return {-45.0f}; } };
