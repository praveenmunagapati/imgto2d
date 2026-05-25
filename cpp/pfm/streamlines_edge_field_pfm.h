#pragma once
#include "pfm/base_streamline_pfm.h"
#include "pfm/pfm_base.h"

class StreamlinesEdgeFieldPFM : public BaseStreamlinePFM {
    Q_OBJECT
public:
    explicit StreamlinesEdgeFieldPFM(QObject* parent = nullptr) : BaseStreamlinePFM(parent) {}
    QString name() const override { return "Streamlines Edge Field"; }
protected:
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
