#pragma once
#include "pfm/adaptive_diagram_pfm.h"
#include "pfm/adaptive_pfm.h"

class LBGDiagramPFM : public AdaptiveDiagramPFM {
    Q_OBJECT
public:
    explicit LBGDiagramPFM(QObject* p = nullptr) : AdaptiveDiagramPFM(p) {}
    QString name() const override { return "LBG Diagram"; }
    QString category() const override { return "LBG"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};
