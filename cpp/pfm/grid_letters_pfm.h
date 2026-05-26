#pragma once
#include "pfm/base_grid_pfm.h"
#include "pfm/pfm_base.h"

class GridLettersPFM : public BaseGridPFM {
    Q_OBJECT
public: explicit GridLettersPFM(QObject* p=nullptr):BaseGridPFM(p){} QString name() const override{return "Grid Letters";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };
