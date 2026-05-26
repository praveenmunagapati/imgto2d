#pragma once
#include "pfm/mosaic_rectangles_pfm.h"
#include "pfm/pfm_base.h"

class MosaicCustomPFM : public MosaicRectanglesPFM {
    Q_OBJECT
public: explicit MosaicCustomPFM(QObject* p=nullptr):MosaicRectanglesPFM(p){} QString name() const override{return "Mosaic Custom";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };
