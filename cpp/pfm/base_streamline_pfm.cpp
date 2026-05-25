#include "pfm/base_streamline_pfm.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

BaseStreamlinePFM::BaseStreamlinePFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> BaseStreamlinePFM::defineSettings() const {
    return {
        {"line_count", "Line Count", SettingType::Integer, 120, QVariant(), 10, 2000, 10, 2000, 50},
        {"max_length", "Max Length", SettingType::Integer, 200, QVariant(), 20, 2000, 20, 2000, 10},
        {"step_size", "Step Size", SettingType::Number, 1.5, QVariant(), 0.5, 5.0, 0.5, 5.0, 0.1}
    };
}
