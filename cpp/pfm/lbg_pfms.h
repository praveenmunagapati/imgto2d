#pragma once

#include "pfm/adaptive_pfm.h"
#include "pfm/adaptive_extras.h"

QVector<PFMSetting> makeLbgSettings(QVector<PFMSetting> settings);

class LBGCircularScribblesPFM : public AdaptiveCircularScribblesPFM {
    Q_OBJECT
public:
    explicit LBGCircularScribblesPFM(QObject* p = nullptr) : AdaptiveCircularScribblesPFM(p) {}
    QString name() const override { return "LBG Circular Scribbles"; }
    QString category() const override { return "LBG"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};

class LBGShapesPFM : public AdaptiveShapesPFM {
    Q_OBJECT
public:
    explicit LBGShapesPFM(QObject* p = nullptr) : AdaptiveShapesPFM(p) {}
    QString name() const override { return "LBG Shapes"; }
    QString category() const override { return "LBG"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};

class LBGTriangulationPFM : public AdaptiveTriangulationPFM {
    Q_OBJECT
public:
    explicit LBGTriangulationPFM(QObject* p = nullptr) : AdaptiveTriangulationPFM(p) {}
    QString name() const override { return "LBG Triangulation"; }
    QString category() const override { return "LBG"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};

class LBGTreePFM : public AdaptiveTreePFM {
    Q_OBJECT
public:
    explicit LBGTreePFM(QObject* p = nullptr) : AdaptiveTreePFM(p) {}
    QString name() const override { return "LBG Tree"; }
    QString category() const override { return "LBG"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};

class LBGStipplingPFM : public AdaptiveStipplingPFM {
    Q_OBJECT
public:
    explicit LBGStipplingPFM(QObject* p = nullptr) : AdaptiveStipplingPFM(p) {}
    QString name() const override { return "LBG Stippling"; }
    QString category() const override { return "LBG"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};

class LBGDashesPFM : public AdaptiveDashesPFM {
    Q_OBJECT
public:
    explicit LBGDashesPFM(QObject* p = nullptr) : AdaptiveDashesPFM(p) {}
    QString name() const override { return "LBG Dashes"; }
    QString category() const override { return "LBG"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};

class LBGDiagramPFM : public AdaptiveDiagramPFM {
    Q_OBJECT
public:
    explicit LBGDiagramPFM(QObject* p = nullptr) : AdaptiveDiagramPFM(p) {}
    QString name() const override { return "LBG Diagram"; }
    QString category() const override { return "LBG"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};

class LBGTSPPFM : public AdaptiveTSPPFM {
    Q_OBJECT
public:
    explicit LBGTSPPFM(QObject* p = nullptr) : AdaptiveTSPPFM(p) {}
    QString name() const override { return "LBG TSP"; }
    QString category() const override { return "LBG"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};
