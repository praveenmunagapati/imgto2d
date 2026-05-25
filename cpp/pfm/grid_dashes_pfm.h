#pragma once
#include "pfm/base_grid_pfm.h"
#include "pfm/pfm_base.h"

class GridDashesPFM : public BaseGridPFM { Q_OBJECT public: explicit GridDashesPFM(QObject* p=nullptr):BaseGridPFM(p){} QString name() const override{return "Grid Dashes";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };
