#pragma once
#include "pfm/base_multi_hatch_pfm.h"
#include "pfm/pfm_base.h"

class Hatch6WayPFM : public BaseMultiHatchPFM { Q_OBJECT public: explicit Hatch6WayPFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch 6-Way";} protected: std::vector<float> getAngles() const override { return {0.0f, 90.0f, 45.0f, -45.0f, 22.5f, -22.5f}; } };
