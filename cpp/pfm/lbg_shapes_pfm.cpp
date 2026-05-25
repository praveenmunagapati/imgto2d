#include "pfm/lbg_shapes_pfm.h"
#include "pfm/lbg_pfms.h"

QVector<PFMSetting> LBGShapesPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveShapesPFM::defineSettings());
}
