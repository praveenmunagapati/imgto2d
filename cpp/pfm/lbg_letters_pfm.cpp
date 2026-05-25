#include "pfm/lbg_letters_pfm.h"
#include <opencv2/imgproc.hpp>
#include <map>
#include <cmath>
#include <iostream>

// -------------------------------------------------------------------------
// LBGLettersPFM
// -------------------------------------------------------------------------
LBGLettersPFM::LBGLettersPFM(QObject* parent) : _LettersBasePFM(parent) {}

int LBGLettersPFM::getLloydIters() const { 
    int iters = m_settings["lloyd_iterations"].toInt();
    return iters > 0 ? iters : 5; 
}

QVector<PFMSetting> LBGLettersPFM::defineSettings() const {
    auto s = _LettersBasePFM::defineSettings();
    for (auto& setting : s) {
        if (setting.key == "lloyd_iterations") {
            setting.defaultVal = 5;
            setting.value = 5;
        }
    }
    return s;
}
