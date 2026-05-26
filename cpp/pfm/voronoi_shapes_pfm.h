#pragma once
#include "pfm/base_voronoi_extra_pfm.h"
#include "pfm/pfm_base.h"

class VoronoiShapesPFM : public BaseVoronoiExtraPFM {
    Q_OBJECT
public: explicit VoronoiShapesPFM(QObject* p=nullptr):BaseVoronoiExtraPFM(p){} QString name() const override{return "Voronoi Shapes";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };
