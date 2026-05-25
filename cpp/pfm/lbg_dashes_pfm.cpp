#include "pfm/lbg_dashes_pfm.h"
#include "pfm/lbg_pfms.h"

QVector<PFMSetting> LBGDashesPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveDashesPFM::defineSettings());
}
