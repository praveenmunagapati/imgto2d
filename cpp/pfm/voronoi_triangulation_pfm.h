#pragma once
#include "pfm/base_voronoi_extra_pfm.h"
#include "pfm/pfm_base.h"

class VoronoiTriangulationPFM : public BaseVoronoiExtraPFM { Q_OBJECT public: explicit VoronoiTriangulationPFM(QObject* p=nullptr):BaseVoronoiExtraPFM(p){} QString name() const override{return "Voronoi Triangulation";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };
