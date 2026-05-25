#include "pfm/lbgtsppfm.h"
#include "pfm/lbg_pfms.h"

QVector<PFMSetting> LBGTSPPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveTSPPFM::defineSettings());
}
