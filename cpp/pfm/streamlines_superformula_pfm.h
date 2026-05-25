#pragma once
#include "pfm/base_streamline_pfm.h"
#include "pfm/pfm_base.h"

class StreamlinesSuperformulaPFM : public BaseStreamlinePFM {
    Q_OBJECT
public:
    explicit StreamlinesSuperformulaPFM(QObject* parent = nullptr) : BaseStreamlinePFM(parent) {}
    QString name() const override { return "Streamlines Superformula"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
