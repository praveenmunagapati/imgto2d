#pragma once
#include "pfm/base_stipple_pfm.h"
#include "pfm/pfm_base.h"

class StippleChaosPFM : public BaseStipplePFM {
    Q_OBJECT
public: explicit StippleChaosPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Chaos";} protected: Path generateShape(float cx, float cy, float r) override; };
