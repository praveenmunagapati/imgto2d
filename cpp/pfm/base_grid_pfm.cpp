#include "pfm/base_grid_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <map>

BaseGridPFM::BaseGridPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> BaseGridPFM::defineSettings() const {
    return {
        {"cols", "Columns", SettingType::Integer, 40, QVariant(), 2, 200, 2, 200, 1},
        {"rows", "Rows", SettingType::Integer, 40, QVariant(), 2, 200, 2, 200, 1},
        {"threshold", "Darkness Threshold", SettingType::Number, 50.0, QVariant(), 0.0, 255.0, 0.0, 255.0, 1.0}
    };
}
