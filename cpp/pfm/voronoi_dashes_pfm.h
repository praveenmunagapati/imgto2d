#pragma once
#include "pfm/base_voronoi_extra_pfm.h"
#include "pfm/pfm_base.h"

class VoronoiDashesPFM : public BaseVoronoiExtraPFM { Q_OBJECT public: explicit VoronoiDashesPFM(QObject* p=nullptr):BaseVoronoiExtraPFM(p){} QString name() const override{return "Voronoi Dashes";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };
