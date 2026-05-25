#pragma once
#include "pfm/pfm_base.h"

class SpiralSawtoothPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SpiralSawtoothPFM(QObject* parent = nullptr);
    QString name() const override { return "Spiral Sawtooth"; }
    QString category() const override { return "Spiral"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class SpiralCircularScribblesPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SpiralCircularScribblesPFM(QObject* parent = nullptr);
    QString name() const override { return "Spiral Circular Scribbles"; }
    QString category() const override { return "Spiral"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
