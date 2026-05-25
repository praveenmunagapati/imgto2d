#pragma once
#include "pfm/base_mosaic_pfm.h"
#include "pfm/pfm_base.h"

class MosaicVoronoiPFM : public BaseMosaicPFM { Q_OBJECT public: explicit MosaicVoronoiPFM(QObject* p=nullptr):BaseMosaicPFM(p){} QString name() const override{return "Mosaic Voronoi";} protected: QVector<PFMSetting> defineSettings() const override; QVector<DrawingGeometry> _process(const cv::Mat& image) override; };
