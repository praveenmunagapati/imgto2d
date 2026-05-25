#pragma once
#include "pfm/base_streamline_pfm.h"
#include "pfm/pfm_base.h"

class StreamlinesFlowFieldPFM : public BaseStreamlinePFM {
    Q_OBJECT
public:
    explicit StreamlinesFlowFieldPFM(QObject* parent = nullptr) : BaseStreamlinePFM(parent) {}
    QString name() const override { return "Streamlines Flow Field"; }
protected:
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
