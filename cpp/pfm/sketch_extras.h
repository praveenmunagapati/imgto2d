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

class SketchQuadBeziers2PFM : public SketchCubicBeziers2PFM {
    Q_OBJECT
public:
    explicit SketchQuadBeziers2PFM(QObject* parent = nullptr) : SketchCubicBeziers2PFM(parent) {}
    QString name() const override { return "Sketch Quad Beziers 2"; }
protected:
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class SketchRadialPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchRadialPFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Radial"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class SketchScribblePFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchScribblePFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Scribble"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class SketchAbstractPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchAbstractPFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Abstract"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class SketchCatmullRomsPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchCatmullRomsPFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Catmull-Roms"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class SketchSobelEdgesPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchSobelEdgesPFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Sobel Edges"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
