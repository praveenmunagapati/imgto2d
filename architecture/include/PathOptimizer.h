#pragma once
#include "PFMBase.h"

namespace DrawingBot {

    class PathOptimizer {
    public:
        // Ramer-Douglas-Peucker line simplification
        static void simplifyPaths(std::vector<PlotPath>& paths, float epsilon = 1.0f);
        
        // Join paths whose endpoints are close to each other
        static void joinPaths(std::vector<PlotPath>& paths, float joinDistance = 2.0f);
        
        // Greedy TSP routing to minimize pen-up travel
        static void optimizeRouting(std::vector<PlotPath>& paths);
    };

} // namespace DrawingBot
