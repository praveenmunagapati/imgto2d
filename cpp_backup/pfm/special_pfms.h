#pragma once
#include "pfm/pfm_base.h"

class ContourPathsPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit ContourPathsPFM(QObject* parent = nullptr);
    QString name() const override { return "Contour Paths"; }
    QString category() const override { return "Special"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class AmbientFlowPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit AmbientFlowPFM(QObject* parent = nullptr);
    QString name() const override { return "Ambient Flow"; }
    QString category() const override { return "Special"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class EdgeShadingPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit EdgeShadingPFM(QObject* parent = nullptr);
    QString name() const override { return "Edge Shading"; }
    QString category() const override { return "Special"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
