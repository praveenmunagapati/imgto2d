#pragma once
#include "pfm/adaptive_triangulation_pfm.h"
#include "pfm/adaptive_pfm.h"

class LBGTriangulationPFM : public AdaptiveTriangulationPFM {
    Q_OBJECT
public:
    explicit LBGTriangulationPFM(QObject* p = nullptr) : AdaptiveTriangulationPFM(p) {}
    QString name() const override { return "LBG Triangulation"; }
    QString category() const override { return "LBG"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};
