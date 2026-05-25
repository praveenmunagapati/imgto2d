#pragma once
/**
 * hpgl_exporter.h — Export path geometries to HPGL format for legacy pen plotters.
 */

#include <QString>
#include <QVector>
#include "pfm/pfm_base.h"
#include "core/coordinates.h"

struct HPGLSettings {
    int xMax = 16158;
    int yMax = 11040;
    bool xMirror = false;
    bool yMirror = false;
    int penNumber = 1;
    int penVelocity = 10;
    double curveFlatness = 0.1;
    double minPathLength = 0.5; // mm
    bool optimizePaths = true;
};

class HPGLExporter {
public:
    static bool exportHPGL(const QString& filepath,
                           const QVector<DrawingGeometry>& geometries,
                           const DrawingAreaConfig& drawingArea,
                           int imageWidth,
                           int imageHeight,
                           const HPGLSettings& settings = HPGLSettings());
};
