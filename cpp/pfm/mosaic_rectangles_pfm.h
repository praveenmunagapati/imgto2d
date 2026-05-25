#pragma once
#include "pfm/base_mosaic_pfm.h"
#include "pfm/pfm_base.h"

class MosaicRectanglesPFM : public BaseMosaicPFM { Q_OBJECT public: explicit MosaicRectanglesPFM(QObject* p=nullptr):BaseMosaicPFM(p){} QString name() const override{return "Mosaic Rectangles";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };
