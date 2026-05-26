#pragma once
#include "pfm/base_stipple_pfm.h"
#include "pfm/pfm_base.h"

class StippleTrianglesPFM : public BaseStipplePFM {
    Q_OBJECT
public: explicit StippleTrianglesPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Triangles";} protected: Path generateShape(float cx, float cy, float r) override; };
