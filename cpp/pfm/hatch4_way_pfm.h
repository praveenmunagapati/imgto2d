#pragma once
#include "pfm/base_multi_hatch_pfm.h"
#include "pfm/pfm_base.h"

class Hatch4WayPFM : public BaseMultiHatchPFM { Q_OBJECT public: explicit Hatch4WayPFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch 4-Way";} protected: std::vector<float> getAngles() const override { return {0.0f, 90.0f, 45.0f, -45.0f}; } };
