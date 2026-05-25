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

QVector<PFMSetting> LBGCircularScribblesPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveCircularScribblesPFM::defineSettings());
}

QVector<PFMSetting> LBGShapesPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveShapesPFM::defineSettings());
}

QVector<PFMSetting> LBGTriangulationPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveTriangulationPFM::defineSettings());
}

QVector<PFMSetting> LBGTreePFM::defineSettings() const {
    return makeLbgSettings(AdaptiveTreePFM::defineSettings());
}

QVector<PFMSetting> LBGStipplingPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveStipplingPFM::defineSettings());
}

QVector<PFMSetting> LBGDashesPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveDashesPFM::defineSettings());
}

QVector<PFMSetting> LBGDiagramPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveDiagramPFM::defineSettings());
}

QVector<PFMSetting> LBGTSPPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveTSPPFM::defineSettings());
}
