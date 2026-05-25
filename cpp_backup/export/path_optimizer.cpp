#include "export/path_optimizer.h"
#include <vector>

QMap<int, QVector<Path>> PathOptimizer::preparePenPaths(
    const QVector<DrawingGeometry>& geometries,
    double minLength,
    bool optimize) 
{
    QMap<int, QVector<Path>> penPaths;
    
    // Group paths by pen index
    for (const auto& geom : geometries) {
        if (geom.path.size() >= 2) {
            penPaths[geom.penIndex].append(geom.path);
        }
    }
    
    // Process each pen layer
    for (auto it = penPaths.begin(); it != penPaths.end(); ++it) {
        std::vector<Path> stdPaths(it.value().begin(), it.value().end());
        
        // Filter short paths
        if (minLength > 0.0) {
            stdPaths = filter_short_paths(stdPaths, minLength);
        }
        
        // Optimize travel path using nearest neighbor
        if (optimize) {
            stdPaths = sort_paths_nearest(stdPaths);
        }
        
        it.value() = QVector<Path>(stdPaths.begin(), stdPaths.end());
    }
    
    return penPaths;
}
