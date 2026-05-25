#pragma once
#include "pfm/pfm_base.h"

class BaseVoronoiExtraPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit BaseVoronoiExtraPFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    std::vector<cv::Point2f> getSeeds(const cv::Mat& image);
};

class VoronoiCirclesPFM : public BaseVoronoiExtraPFM { Q_OBJECT public: explicit VoronoiCirclesPFM(QObject* p=nullptr):BaseVoronoiExtraPFM(p){} QString name() const override{return "Voronoi Circles";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };
class VoronoiTriangulationPFM : public BaseVoronoiExtraPFM { Q_OBJECT public: explicit VoronoiTriangulationPFM(QObject* p=nullptr):BaseVoronoiExtraPFM(p){} QString name() const override{return "Voronoi Triangulation";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };
class VoronoiStipplingPFM : public BaseVoronoiExtraPFM { Q_OBJECT public: explicit VoronoiStipplingPFM(QObject* p=nullptr):BaseVoronoiExtraPFM(p){} QString name() const override{return "Voronoi Stippling";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };
class VoronoiDashesPFM : public BaseVoronoiExtraPFM { Q_OBJECT public: explicit VoronoiDashesPFM(QObject* p=nullptr):BaseVoronoiExtraPFM(p){} QString name() const override{return "Voronoi Dashes";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };
class VoronoiDiagramPFM : public BaseVoronoiExtraPFM { Q_OBJECT public: explicit VoronoiDiagramPFM(QObject* p=nullptr):BaseVoronoiExtraPFM(p){} QString name() const override{return "Voronoi Diagram";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };
class VoronoiShapesPFM : public BaseVoronoiExtraPFM { Q_OBJECT public: explicit VoronoiShapesPFM(QObject* p=nullptr):BaseVoronoiExtraPFM(p){} QString name() const override{return "Voronoi Shapes";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };
class VoronoiTreePFM : public BaseVoronoiExtraPFM { Q_OBJECT public: explicit VoronoiTreePFM(QObject* p=nullptr):BaseVoronoiExtraPFM(p){} QString name() const override{return "Voronoi Tree";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };
class VoronoiTSPPFM : public BaseVoronoiExtraPFM { Q_OBJECT public: explicit VoronoiTSPPFM(QObject* p=nullptr):BaseVoronoiExtraPFM(p){} QString name() const override{return "Voronoi TSP";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };
