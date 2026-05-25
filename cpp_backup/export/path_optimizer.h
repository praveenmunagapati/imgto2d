#pragma once
/**
 * path_optimizer.h — Path filtering and optimization (TSP/Nearest Neighbor).
 */

#include <QVector>
#include <QMap>
#include "pfm/pfm_base.h"
#include "core/geometry.h"

class PathOptimizer {
public:
    static QMap<int, QVector<Path>> preparePenPaths(
        const QVector<DrawingGeometry>& geometries,
        double minLength = 0.0,
        bool optimize = true);
};
