#pragma once
#include "pfm/base_voronoi_extra_pfm.h"
#include "pfm/pfm_base.h"

class VoronoiCirclesPFM : public BaseVoronoiExtraPFM {
    Q_OBJECT
public: explicit VoronoiCirclesPFM(QObject* p=nullptr):BaseVoronoiExtraPFM(p){} QString name() const override{return "Voronoi Circles";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };
