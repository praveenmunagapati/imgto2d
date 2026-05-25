#include "pfm/base_mosaic_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>

#include <opencv2/imgproc.hpp>

BaseMosaicPFM::BaseMosaicPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> BaseMosaicPFM::defineSettings() const {
    return {
        {"cols", "Columns", SettingType::Integer, 20, QVariant(), 2, 100, 2, 100, 1},
        {"rows", "Rows", SettingType::Integer, 20, QVariant(), 2, 100, 2, 100, 1},
        {"threshold", "Darkness Threshold", SettingType::Number, 40.0, QVariant(), 0.0, 255.0, 0.0, 255.0, 1.0}
    };
}
