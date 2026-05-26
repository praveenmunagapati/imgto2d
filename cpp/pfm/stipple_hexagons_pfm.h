#pragma once
#include "pfm/base_stipple_pfm.h"
#include "pfm/pfm_base.h"

class StippleHexagonsPFM : public BaseStipplePFM {
    Q_OBJECT
public: explicit StippleHexagonsPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Hexagons";} protected: Path generateShape(float cx, float cy, float r) override; };
