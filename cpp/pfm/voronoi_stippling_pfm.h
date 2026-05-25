#pragma once
#include "pfm/base_voronoi_extra_pfm.h"
#include "pfm/pfm_base.h"

class VoronoiStipplingPFM : public BaseVoronoiExtraPFM { Q_OBJECT public: explicit VoronoiStipplingPFM(QObject* p=nullptr):BaseVoronoiExtraPFM(p){} QString name() const override{return "Voronoi Stippling";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };
