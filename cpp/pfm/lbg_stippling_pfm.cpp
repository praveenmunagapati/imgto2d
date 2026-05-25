#include "pfm/lbg_stippling_pfm.h"
#include "pfm/lbg_pfms.h"

QVector<PFMSetting> LBGStipplingPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveStipplingPFM::defineSettings());
}
