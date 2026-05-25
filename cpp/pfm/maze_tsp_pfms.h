#pragma once
#include "pfm/pfm_base.h"

class BaseTSPPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit BaseTSPPFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    virtual std::vector<double> getProbabilities(const cv::Mat& image) = 0;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class TSPClassicPFM : public BaseTSPPFM {
    Q_OBJECT
public:
    explicit TSPClassicPFM(QObject* parent = nullptr) : BaseTSPPFM(parent) {}
    QString name() const override { return "TSP Classic"; }
protected:
    std::vector<double> getProbabilities(const cv::Mat& image) override;
};

class TSPOutlinePFM : public BaseTSPPFM {
    Q_OBJECT
public:
    explicit TSPOutlinePFM(QObject* parent = nullptr) : BaseTSPPFM(parent) {}
    QString name() const override { return "TSP Outline"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    std::vector<double> getProbabilities(const cv::Mat& image) override;
};

class TSPShadingPFM : public BaseTSPPFM {
    Q_OBJECT
public:
    explicit TSPShadingPFM(QObject* parent = nullptr) : BaseTSPPFM(parent) {}
    QString name() const override { return "TSP Shading"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    std::vector<double> getProbabilities(const cv::Mat& image) override;
};

class TSPMSTPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit TSPMSTPFM(QObject* parent = nullptr);
    QString name() const override { return "Minimum Spanning Tree"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class TSPStipplePFM : public BaseTSPPFM {
    Q_OBJECT
public:
    explicit TSPStipplePFM(QObject* parent = nullptr) : BaseTSPPFM(parent) {}
    QString name() const override { return "TSP Stipple"; }
protected:
    std::vector<double> getProbabilities(const cv::Mat& image) override;
};

class TSPVoronoiPFM : public BaseTSPPFM {
    Q_OBJECT
public:
    explicit TSPVoronoiPFM(QObject* parent = nullptr);
    QString name() const override { return "TSP Voronoi"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    std::vector<double> getProbabilities(const cv::Mat& image) override;
};

// Phase 7: Mazes
class BaseMazePFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit BaseMazePFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
    virtual std::pair<float, float> transformPoint(float x, float y);
};

class MazeRectPFM : public BaseMazePFM { Q_OBJECT public: explicit MazeRectPFM(QObject* p=nullptr):BaseMazePFM(p){} QString name() const override{return "Maze Rectangular";} };
class MazeHexPFM : public BaseMazePFM { Q_OBJECT public: explicit MazeHexPFM(QObject* p=nullptr):BaseMazePFM(p){} QString name() const override{return "Maze Hexagonal";} protected: std::pair<float, float> transformPoint(float x, float y) override; };
class MazeTriPFM : public BaseMazePFM { Q_OBJECT public: explicit MazeTriPFM(QObject* p=nullptr):BaseMazePFM(p){} QString name() const override{return "Maze Triangular";} protected: std::pair<float, float> transformPoint(float x, float y) override; };
class MazeVoronoiPFM : public BaseMazePFM { Q_OBJECT public: explicit MazeVoronoiPFM(QObject* p=nullptr):BaseMazePFM(p){} QString name() const override{return "Maze Voronoi";} protected: std::pair<float, float> transformPoint(float x, float y) override; };
class MazeCirclePFM : public BaseMazePFM { Q_OBJECT public: explicit MazeCirclePFM(QObject* p=nullptr):BaseMazePFM(p){} QString name() const override{return "Maze Circular";} protected: std::pair<float, float> transformPoint(float x, float y) override; };
class MazeLabyrinthPFM : public BaseMazePFM { Q_OBJECT public: explicit MazeLabyrinthPFM(QObject* p=nullptr):BaseMazePFM(p){} QString name() const override{return "Labyrinth";} };

class LabyrinthClassicPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit LabyrinthClassicPFM(QObject* parent = nullptr);
    QString name() const override { return "Labyrinth Classic"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class LabyrinthRomanPFM : public LabyrinthClassicPFM {
    Q_OBJECT
public:
    explicit LabyrinthRomanPFM(QObject* p = nullptr) : LabyrinthClassicPFM(p) {}
    QString name() const override { return "Labyrinth Roman"; }
};

class MazeHilbertPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit MazeHilbertPFM(QObject* parent = nullptr);
    QString name() const override { return "Hilbert Curve"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class MazePeanoPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit MazePeanoPFM(QObject* parent = nullptr);
    QString name() const override { return "Peano Curve"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
