#pragma once
#include "pfm/pfm_base.h"

class _LettersBasePFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit _LettersBasePFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    virtual int getLloydIters() const = 0;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
