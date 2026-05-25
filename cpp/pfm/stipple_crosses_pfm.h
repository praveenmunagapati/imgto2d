#pragma once
#include "pfm/base_stipple_pfm.h"
#include "pfm/pfm_base.h"

class StippleCrossesPFM : public BaseStipplePFM { Q_OBJECT public: explicit StippleCrossesPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Crosses";} protected: Path generateShape(float cx, float cy, float r) override; };
