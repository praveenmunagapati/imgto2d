#include "geometry.h"
#include "coordinates.h"
#pragma once
/**
 * path_optimizer.h — Path filtering and optimization (TSP/Nearest Neighbor).
 */



#include "core_types.h"
#include "core_types.h"

class PathOptimizer {
public:
    static std::map<int, std::vector<Path>> preparePenPaths(
        const std::vector<DrawingGeometry>& geometries,
        double minLength = 0.0,
        bool optimize = true,
        double simplifyTolerance = 0.0,
        double mergeTolerance = 0.0);
};
