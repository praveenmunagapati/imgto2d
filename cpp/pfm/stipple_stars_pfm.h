#pragma once
#include "pfm/base_stipple_pfm.h"
#include "pfm/pfm_base.h"

class StippleStarsPFM : public BaseStipplePFM {
    Q_OBJECT
public: explicit StippleStarsPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Stars";} protected: Path generateShape(float cx, float cy, float r) override; };
