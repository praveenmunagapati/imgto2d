#pragma once
#include "pfm/base_voronoi_extra_pfm.h"
#include "pfm/pfm_base.h"

class VoronoiTSPPFM : public BaseVoronoiExtraPFM { Q_OBJECT public: explicit VoronoiTSPPFM(QObject* p=nullptr):BaseVoronoiExtraPFM(p){} QString name() const override{return "Voronoi TSP";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };
