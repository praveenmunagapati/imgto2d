#pragma once
#include "pfm/pfm_base.h"

class SketchSuperformulaPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchSuperformulaPFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Superformula"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class SketchCubicBeziers2PFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchCubicBeziers2PFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Cubic Beziers 2"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class SketchSweepingCurvesPFM : public SketchCubicBeziers2PFM {
    Q_OBJECT
public:
    explicit SketchSweepingCurvesPFM(QObject* parent = nullptr);
    QString name() const override;
};

class SketchFlowFieldsPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchFlowFieldsPFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Flow Fields"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class SketchVoronoiPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchVoronoiPFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Voronoi"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class SketchDelaunayPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchDelaunayPFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Delaunay"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
