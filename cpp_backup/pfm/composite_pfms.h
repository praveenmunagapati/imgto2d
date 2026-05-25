#pragma once
/**
 * composite_pfms.h — Multi-layer composite PFMs.
 * Port of app/pfm/composite_pfms.py
 */
#include "pfm/pfm_base.h"

// Runs SketchLines on multiple brightness layers (CMYK-style separation)
class LayersPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit LayersPFM(QObject* parent = nullptr);
    QString name()     const override { return "Layers PFM"; }
    QString category() const override { return "Composite"; }
    bool isPremium()   const override { return true; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

// Runs StippleDots on multiple brightness bands
class StippleLayersPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit StippleLayersPFM(QObject* parent = nullptr);
    QString name()     const override { return "Stipple Layers"; }
    QString category() const override { return "Composite"; }
    bool isPremium()   const override { return true; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
