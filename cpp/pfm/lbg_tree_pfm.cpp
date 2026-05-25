#include "pfm/lbg_tree_pfm.h"
#include "pfm/lbg_pfms.h"

QVector<PFMSetting> LBGTreePFM::defineSettings() const {
    return makeLbgSettings(AdaptiveTreePFM::defineSettings());
}
