#pragma once
/**
 * adaptive_extras.h — Full Adaptive PFM family (port from adaptive_pfms.py).
 * All share the same Adaptive base settings: cell_count, lloyd_iterations,
 * min_brightness, plotting_resolution, random_seed.
 */
#include "pfm/pfm_base.h"

// ---------------------------------------------------------------------------
// Shared base
// ---------------------------------------------------------------------------
class BaseAdaptivePFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit BaseAdaptivePFM(QObject* parent = nullptr);
    QString category() const override { return "Adaptive"; }
    bool    isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    // Returns brightness-weighted, Lloyd-relaxed seed points
    std::vector<cv::Point2f> getSeeds(const cv::Mat& image);
};

// ---------------------------------------------------------------------------
// Concrete Adaptive PFMs
// ---------------------------------------------------------------------------
class AdaptiveCircularScribblesPFM : public BaseAdaptivePFM {
    Q_OBJECT
public:
    explicit AdaptiveCircularScribblesPFM(QObject* p = nullptr);
    QString name() const override { return "Adaptive Circular Scribbles"; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class AdaptiveShapesPFM : public BaseAdaptivePFM {
    Q_OBJECT
public:
    explicit AdaptiveShapesPFM(QObject* p = nullptr);
    QString name() const override { return "Adaptive Shapes"; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class AdaptiveDashesPFM : public BaseAdaptivePFM {
    Q_OBJECT
public:
    explicit AdaptiveDashesPFM(QObject* p = nullptr) : BaseAdaptivePFM(p) {}
    QString name() const override { return "Adaptive Dashes"; }
protected:
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class AdaptiveTSPPFM : public BaseAdaptivePFM {
    Q_OBJECT
public:
    explicit AdaptiveTSPPFM(QObject* p = nullptr) : BaseAdaptivePFM(p) {}
    QString name() const override { return "Adaptive TSP"; }
protected:
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class AdaptiveTriangulationPFM : public BaseAdaptivePFM {
    Q_OBJECT
public:
    explicit AdaptiveTriangulationPFM(QObject* p = nullptr) : BaseAdaptivePFM(p) {}
    QString name() const override { return "Adaptive Triangulation"; }
protected:
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class AdaptiveTreePFM : public BaseAdaptivePFM {
    Q_OBJECT
public:
    explicit AdaptiveTreePFM(QObject* p = nullptr) : BaseAdaptivePFM(p) {}
    QString name() const override { return "Adaptive Tree"; }
protected:
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class AdaptiveDiagramPFM : public BaseAdaptivePFM {
    Q_OBJECT
public:
    explicit AdaptiveDiagramPFM(QObject* p = nullptr) : BaseAdaptivePFM(p) {}
    QString name() const override { return "Adaptive Diagram"; }
protected:
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
