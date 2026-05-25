#pragma once
/**
 * svg_exporter.h — Export path geometries to an SVG file.
 */

#include <QString>
#include <QVector>
#include <QColor>
#include "pfm/pfm_base.h"
#include "core/coordinates.h"

class SVGExporter {
public:
    static bool exportSVG(const QString& filepath,
                          const QVector<DrawingGeometry>& geometries,
                          const DrawingAreaConfig& drawingArea,
                          int imageWidth,
                          int imageHeight,
                          const QVector<QColor>& penColors,
                          double penWidthMm = 0.5);
};
