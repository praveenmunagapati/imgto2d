#pragma once
#include "pfm/base_grid_pfm.h"
#include "pfm/pfm_base.h"

class GridShapesPFM : public BaseGridPFM {
    Q_OBJECT
public: explicit GridShapesPFM(QObject* p=nullptr):BaseGridPFM(p){} QString name() const override{return "Grid Shapes";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };
