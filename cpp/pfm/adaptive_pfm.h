#pragma once
#include "pfm/pfm_base.h"

/**
 * adaptive_pfm.h — Adaptive Stippling PFM (brightness-weighted dot placement).
 * This is the "Adaptive Stippling" class from tk_gui_fast.py's AVAILABLE_PFMS list.
 * It is a simplified C++ implementation: weighted random sampling without
 * the full Lloyd relaxation from the Python pfm_utils (which has heavy scipy deps).
 */
class AdaptiveStipplingPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit AdaptiveStipplingPFM(QObject* parent = nullptr);
    QString name()        const override { return "Adaptive Stippling"; }
    QString description() const override { return "Brightness-weighted adaptive stipple placement."; }
    QString category()    const override { return "Adaptive"; }
    bool    isPremium()   const override { return true; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
