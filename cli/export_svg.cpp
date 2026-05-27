#include "export_svg.h"
#include "coordinates.h"
#include "path_optimizer.h"
#include <fstream>
#include <iostream>
#include <map>
#include <iomanip>
#include <sstream>

static std::string format_coord(const std::string& fmt, double x, double y) {
    char buf[128];
    snprintf(buf, sizeof(buf), fmt.c_str(), x, y);
    return std::string(buf);
}

void export_svg(const std::string& filename,
                const std::vector<DrawingGeometry>& geometries,
                const DrawingAreaConfig& drawingArea,
                int imageWidth,
                int imageHeight,
                const std::vector<std::string>& penColorsHex,
                double penWidthMm,
                double simplifyTolerance) {
    if (geometries.empty()) return;

    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Failed to open " << filename << " for writing.\n";
        return;
    }

    // Write SVG header
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" "
        << "xmlns:inkscape=\"http://www.inkscape.org/namespaces/inkscape\" "
        << "width=\"" << drawingArea.width_mm << "mm\" height=\"" << drawingArea.height_mm << "mm\" "
        << "viewBox=\"0 0 " << drawingArea.width_mm << " " << drawingArea.height_mm << "\">\n";

    // Convert pixel coordinates to physical mm
    std::vector<DrawingGeometry> mmGeoms;
    mmGeoms.reserve(geometries.size());
    for (const auto& geom : geometries) {
        Path mmPath = pixel_path_to_mm(geom.path, imageWidth, imageHeight, drawingArea);
        if (!mmPath.empty()) {
            mmGeoms.push_back({mmPath, geom.penIndex, geom.groupIndex});
        }
    }

    // Prepare and simplify paths
    std::map<int, std::vector<Path>> penPaths = PathOptimizer::preparePenPaths(
        mmGeoms, 0.0, true, simplifyTolerance);

    // Write groups
    for (auto it = penPaths.begin(); it != penPaths.end(); ++it) {
        int penIdx = it->first;
        std::string color = (penIdx >= 0 && penIdx < penColorsHex.size()) ? penColorsHex[penIdx] : "#000000";
        
        out << "  <g id=\"pen_" << penIdx << "\" inkscape:label=\"Pen " << penIdx << "\">\n";
        
        for (const auto& path : it->second) {
            if (path.size() < 2) continue;
            
            std::string d = format_coord("M%.3f,%.3f", path[0].first, path[0].second);
            for (std::size_t i = 1; i < path.size(); ++i) {
                d += format_coord(" L%.3f,%.3f", path[i].first, path[i].second);
            }
            
            out << "    <path d=\"" << d << "\" fill=\"none\" stroke=\"" << color << "\" "
                << "stroke-width=\"" << penWidthMm << "\" stroke-linecap=\"round\" "
                << "stroke-linejoin=\"round\"/>\n";
        }
        
        out << "  </g>\n";
    }

    out << "</svg>\n";
}
