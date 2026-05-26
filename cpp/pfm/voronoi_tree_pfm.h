#pragma once
#include "pfm/base_voronoi_extra_pfm.h"
#include "pfm/pfm_base.h"

class VoronoiTreePFM : public BaseVoronoiExtraPFM {
    Q_OBJECT
public: explicit VoronoiTreePFM(QObject* p=nullptr):BaseVoronoiExtraPFM(p){} QString name() const override{return "Voronoi Tree";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };
