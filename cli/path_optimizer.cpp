#include "path_optimizer.h"
#include <vector>

std::map<int, std::vector<Path>> PathOptimizer::preparePenPaths(
    const std::vector<DrawingGeometry>& geometries,
    double minLength,
    bool optimize,
    double simplifyTolerance) 
{
    std::map<int, std::vector<Path>> penPaths;
    
    // Group paths by pen index
    for (const auto& geom : geometries) {
        if (geom.path.size() >= 2) {
            penPaths[geom.penIndex].push_back(geom.path);
        }
    }
    
    // Process each pen layer
    for (auto it = penPaths.begin(); it != penPaths.end(); ++it) {
        std::vector<Path> stdPaths(it->second.begin(), it->second.end());
        
        // Filter short paths
        if (minLength > 0.0) {
            stdPaths = filter_short_paths(stdPaths, minLength);
        }
        
        // Simplify paths using Douglas-Peucker
        if (simplifyTolerance > 0.0) {
            for (auto& path : stdPaths) {
                path = simplify_path_dp(path, simplifyTolerance);
            }
        }
        
        // Optimize travel path using nearest neighbor
        if (optimize) {
            stdPaths = sort_paths_nearest(stdPaths);
        }
        
        it->second = std::vector<Path>(stdPaths.begin(), stdPaths.end());
    }
    
    return penPaths;
}
