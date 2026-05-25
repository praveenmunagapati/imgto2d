#include "pfm/voronoi_letters_pfm.h"
#include <opencv2/imgproc.hpp>
#include <map>
#include <cmath>
#include <iostream>

// -------------------------------------------------------------------------
// VoronoiLettersPFM
// -------------------------------------------------------------------------
int VoronoiLettersPFM::getLloydIters() const {
    return m_settings["lloyd_iterations"].toInt();
}
