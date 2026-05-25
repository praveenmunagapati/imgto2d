#include "pfm/lbg_circular_scribbles_pfm.h"
#include "pfm/lbg_pfms.h"

QVector<PFMSetting> LBGCircularScribblesPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveCircularScribblesPFM::defineSettings());
}
