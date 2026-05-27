#include "hpgl_exporter.h"

static std::string join_segments(const std::vector<std::string>& vec, const std::string& delim) {
    std::string res;
    for(size_t i=0; i<vec.size(); ++i) {
        res += vec[i];
        if(i != vec.size()-1) res += delim;
    }
    return res;
}


#include <iomanip>
#include <sstream>

static std::string format_coord(const std::string& fmt, double x, double y) {
    char buf[128];
    snprintf(buf, sizeof(buf), fmt.c_str(), x, y);
    return std::string(buf);
}

#include "path_optimizer.h"




static std::pair<int, int> mmToHPGL(double xMm, double yMm, const DrawingAreaConfig& da, const HPGLSettings& settings) {
    double wMm = std::max(da.width_mm, 0.001);
    double hMm = std::max(da.height_mm, 0.001);

    double rx = xMm;
    double ry = yMm;
    double targetWMm = wMm;
    double targetHMm = hMm;

    // Apply rotation
    if (settings.rotation == 90) {
        rx = yMm;
        ry = wMm - xMm;
        targetWMm = hMm;
        targetHMm = wMm;
    } else if (settings.rotation == 180) {
        rx = wMm - xMm;
        ry = hMm - yMm;
    } else if (settings.rotation == 270) {
        rx = hMm - yMm;
        ry = xMm;
        targetWMm = hMm;
        targetHMm = wMm;
    }

    int x = static_cast<int>((rx / targetWMm) * settings.xMax);
    int y = static_cast<int>((ry / targetHMm) * settings.yMax);

    if (settings.xMirror) x = settings.xMax - x;
    if (settings.yMirror) y = settings.yMax - y;

    return {x, y};
}

bool HPGLExporter::exportHPGL(const std::string& filepath,
                              const std::vector<DrawingGeometry>& geometries,
                              const DrawingAreaConfig& drawingArea,
                              int imageWidth,
                              int imageHeight,
                              const HPGLSettings& settings) {
    if (geometries.empty()) return false;

    std::ofstream out(filepath);
    if (!out.is_open()) {
        return false;
    }

    // Convert pixel coordinates to physical mm
    std::vector<DrawingGeometry> mmGeoms;
    mmGeoms.reserve(geometries.size());
    for (const auto& geom : geometries) {
        Path mmPath = pixel_path_to_mm(geom.path, imageWidth, imageHeight, drawingArea);
        if (!mmPath.empty()) {
            mmGeoms.push_back({mmPath, geom.penIndex, geom.groupIndex});
        }
    }

    // Prepare, filter and optimize paths
    std::map<int, std::vector<Path>> penPaths = PathOptimizer::preparePenPaths(
        mmGeoms, settings.minPathLength, settings.optimizePaths, settings.simplifyTolerance);

    double h_mm = drawingArea.height_mm;

    out << "IN;\n";
    out << "SP" << settings.penNumber << ";\n";
    out << "VS" << settings.penVelocity << ";\n";
    if (settings.penForce >= 0) {
        out << "FS" << settings.penForce << ";\n";
    } else {
        out << format_coord("FS%.2f;\n", settings.curveFlatness, 0.0);
    }

    for (auto it = penPaths.begin(); it != penPaths.end(); ++it) {
        int penIdx = it->first;
        out << "; Layer: Pen " << penIdx << "\n";

        for (const auto& path : it->second) {
            if (path.size() < 2) continue;

            auto [hx0, hy0] = mmToHPGL(path[0].first, h_mm - path[0].second, drawingArea, settings);
            out << format_coord("PU%.0f,%.0f;\n", hx0, hy0);

            std::vector<std::string> segments;
            for (std::size_t i = 1; i < path.size(); ++i) {
                auto [hx, hy] = mmToHPGL(path[i].first, h_mm - path[i].second, drawingArea, settings);
                segments.push_back(std::to_string(hx) + "," + std::to_string(hy));
            }

            if (!segments.empty()) {
                out << "PD" << join_segments(segments, ",") << ";\n";
            }
        }
    }

    out << "SP0;\n";
    return true;
}
