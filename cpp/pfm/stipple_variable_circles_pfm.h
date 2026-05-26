#pragma once
#include "pfm/base_stipple_pfm.h"
#include "pfm/pfm_base.h"

class StippleVariableCirclesPFM : public BaseStipplePFM {
    Q_OBJECT
public: explicit StippleVariableCirclesPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Variable Circles";} protected: Path generateShape(float cx, float cy, float r) override; };
