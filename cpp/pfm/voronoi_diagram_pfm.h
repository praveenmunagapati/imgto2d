#pragma once
#include "pfm/base_voronoi_extra_pfm.h"
#include "pfm/pfm_base.h"

class VoronoiDiagramPFM : public BaseVoronoiExtraPFM { Q_OBJECT public: explicit VoronoiDiagramPFM(QObject* p=nullptr):BaseVoronoiExtraPFM(p){} QString name() const override{return "Voronoi Diagram";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };
