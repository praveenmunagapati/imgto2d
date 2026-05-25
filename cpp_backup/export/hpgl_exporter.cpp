#include "export/hpgl_exporter.h"
#include "export/path_optimizer.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>

static std::pair<int, int> mmToHPGL(double xMm, double yMm, const DrawingAreaConfig& da, const HPGLSettings& settings) {
    double wMm = std::max(da.width_mm, 0.001);
    double hMm = std::max(da.height_mm, 0.001);

    int x = static_cast<int>((xMm / wMm) * settings.xMax);
    int y = static_cast<int>((yMm / hMm) * settings.yMax);

    if (settings.xMirror) x = settings.xMax - x;
    if (settings.yMirror) y = settings.yMax - y;

    return {x, y};
}

bool HPGLExporter::exportHPGL(const QString& filepath,
                              const QVector<DrawingGeometry>& geometries,
                              const DrawingAreaConfig& drawingArea,
                              int imageWidth,
                              int imageHeight,
                              const HPGLSettings& settings) {
    if (geometries.isEmpty()) return false;

    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);

    // Convert pixel coordinates to physical mm
    QVector<DrawingGeometry> mmGeoms;
    mmGeoms.reserve(geometries.size());
    for (const auto& geom : geometries) {
        Path mmPath = pixel_path_to_mm(geom.path, imageWidth, imageHeight, drawingArea);
        if (!mmPath.empty()) {
            mmGeoms.append({mmPath, geom.penIndex, geom.groupIndex});
        }
    }

    // Prepare, filter and optimize paths
    QMap<int, QVector<Path>> penPaths = PathOptimizer::preparePenPaths(
        mmGeoms, settings.minPathLength, settings.optimizePaths);

    double h_mm = drawingArea.height_mm;

    out << "IN;\n";
    out << QString("SP%1;\n").arg(settings.penNumber);
    out << QString("VS%1;\n").arg(settings.penVelocity);
    out << QString("FS%1;\n").arg(settings.curveFlatness, 0, 'f', 2);

    for (auto it = penPaths.begin(); it != penPaths.end(); ++it) {
        int penIdx = it.key();
        out << "; Layer: Pen " << penIdx << "\n";

        for (const auto& path : it.value()) {
            if (path.size() < 2) continue;

            auto [hx0, hy0] = mmToHPGL(path[0].first, h_mm - path[0].second, drawingArea, settings);
            out << QString("PU%1,%2;\n").arg(hx0).arg(hy0);

            QStringList segments;
            for (std::size_t i = 1; i < path.size(); ++i) {
                auto [hx, hy] = mmToHPGL(path[i].first, h_mm - path[i].second, drawingArea, settings);
                segments.append(QString("%1,%2").arg(hx).arg(hy));
            }

            if (!segments.isEmpty()) {
                out << "PD" << segments.join(",") << ";\n";
            }
        }
    }

    out << "SP0;\n";
    return true;
}
