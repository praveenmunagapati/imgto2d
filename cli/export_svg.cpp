#include "export_svg.h"
#include <fstream>
#include <iostream>

void export_svg(const std::string& filename, const std::vector<DrawingGeometry>& geometries, int width, int height) {
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Failed to open " << filename << " for writing." << std::endl;
        return;
    }

    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" ";
    out << "width=\"" << width << "\" height=\"" << height << "\" ";
    out << "viewBox=\"0 0 " << width << " " << height << "\">\n";
    out << "  <g fill=\"none\" stroke=\"black\" stroke-width=\"1.0\">\n";

    for (const auto& geom : geometries) {
        if (geom.path.empty()) continue;
        out << "    <path d=\"M " << geom.path[0].first << " " << geom.path[0].second;
        for (std::size_t i = 1; i < geom.path.size(); ++i) {
            out << " L " << geom.path[i].first << " " << geom.path[i].second;
        }
        out << "\" />\n";
    }

    out << "  </g>\n";
    out << "</svg>\n";
    std::cout << "Saved SVG to " << filename << std::endl;
}

// ===========================================================================
