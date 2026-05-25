#include "pfm/lbg_diagram_pfm.h"
#include "pfm/lbg_pfms.h"

QVector<PFMSetting> LBGDiagramPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveDiagramPFM::defineSettings());
}
