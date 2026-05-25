#include "pfm/lbg_pfms.h"

QVector<PFMSetting> makeLbgSettings(QVector<PFMSetting> settings) {
    for (auto& setting : settings) {
        if (setting.key == "lloyd_iterations") {
            setting.defaultVal = 5;
            setting.value = 5;
        }
    }
    return settings;
}
