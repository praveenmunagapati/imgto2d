#pragma once
#include "pfm/base_stipple_pfm.h"
#include "pfm/pfm_base.h"

class StippleLinesPFM : public BaseStipplePFM { Q_OBJECT public: explicit StippleLinesPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Lines";} protected: Path generateShape(float cx, float cy, float r) override; };
