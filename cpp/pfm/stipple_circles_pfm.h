#pragma once
#include "pfm/base_stipple_pfm.h"
#include "pfm/pfm_base.h"

class StippleCirclesPFM : public BaseStipplePFM { Q_OBJECT public: explicit StippleCirclesPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Circles";} protected: Path generateShape(float cx, float cy, float r) override; };
