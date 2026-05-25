#pragma once
#include "pfm/base_stipple_pfm.h"
#include "pfm/pfm_base.h"

class StippleVariableSquaresPFM : public BaseStipplePFM { Q_OBJECT public: explicit StippleVariableSquaresPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Variable Squares";} protected: Path generateShape(float cx, float cy, float r) override; };
