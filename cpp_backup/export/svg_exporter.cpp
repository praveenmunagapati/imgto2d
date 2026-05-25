#include "export/svg_exporter.h"
#include <QFile>
#include <QTextStream>
#include <QMap>

bool SVGExporter::exportSVG(const QString& filepath,
                            const QVector<DrawingGeometry>& geometries,
                            const DrawingAreaConfig& drawingArea,
                            int imageWidth,
                            int imageHeight,
                            const QVector<QColor>& penColors,
                            double penWidthMm) {
    if (geometries.isEmpty()) return false;

    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);

    // Write SVG header
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    out << QString("<svg xmlns=\"http://www.w3.org/2000/svg\" "
                   "xmlns:inkscape=\"http://www.inkscape.org/namespaces/inkscape\" "
                   "width=\"%1mm\" height=\"%2mm\" "
                   "viewBox=\"0 0 %1 %2\">\n")
               .arg(drawingArea.width_mm).arg(drawingArea.height_mm);

    // Group paths by pen index
    QMap<int, QVector<Path>> penPaths;
    for (const auto& geom : geometries) {
        Path mmPath = pixel_path_to_mm(geom.path, imageWidth, imageHeight, drawingArea);
        if (!mmPath.empty()) {
            penPaths[geom.penIndex].append(mmPath);
        }
    }

    // Write groups
    for (auto it = penPaths.begin(); it != penPaths.end(); ++it) {
        int penIdx = it.key();
        QColor color = (penIdx >= 0 && penIdx < penColors.size()) ? penColors[penIdx] : QColor(0, 0, 0);
        
        out << QString("  <g id=\"pen_%1\" inkscape:label=\"Pen %1\">\n").arg(penIdx);
        
        for (const auto& path : it.value()) {
            if (path.size() < 2) continue;
            
            QString d = QString("M%1,%2").arg(path[0].first, 0, 'f', 3).arg(path[0].second, 0, 'f', 3);
            for (std::size_t i = 1; i < path.size(); ++i) {
                d += QString(" L%1,%2").arg(path[i].first, 0, 'f', 3).arg(path[i].second, 0, 'f', 3);
            }
            
            out << QString("    <path d=\"%1\" fill=\"none\" stroke=\"%2\" "
                           "stroke-width=\"%3\" stroke-linecap=\"round\" "
                           "stroke-linejoin=\"round\"/>\n")
                       .arg(d).arg(color.name()).arg(penWidthMm, 0, 'f', 4);
        }
        
        out << "  </g>\n";
    }

    out << "</svg>\n";
    return true;
}
