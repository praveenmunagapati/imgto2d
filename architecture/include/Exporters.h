#pragma once
#include "PFMBase.h"
#include <string>
#include <vector>
#include <opencv2/core.hpp>

namespace DrawingBot {

    class Exporter {
    public:
        static void exportSVG(const std::vector<PlotPath>& paths, const std::string& filename, int width, int height);
        static void exportGCode(const std::vector<PlotPath>& paths, const std::string& filename, float feedrate = 3000.0f);
        static void exportHPGL(const std::vector<PlotPath>& paths, const std::string& filename);
    };

} // namespace DrawingBot
