#include "pfm/lbg_triangulation_pfm.h"
#include "pfm/lbg_pfms.h"

QVector<PFMSetting> LBGTriangulationPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveTriangulationPFM::defineSettings());
}
