#pragma once
/**
 * gcode_exporter.h — Export path geometries to standard G-code for CNC/plotters.
 */

#include <QString>
#include <QVector>
#include <QColor>
#include "pfm/pfm_base.h"
#include "core/coordinates.h"

struct GCodeSettings {
    QString startGcode = "G21\nG90";
    QString endGcode = "M30";
    QString penDownCmd = "M3 S90";
    QString penUpCmd = "M5";
    QString startLayerCmd = "";
    QString endLayerCmd = "";
    double xOffset = 0.0;
    double yOffset = 0.0;
    bool centerZero = false;
    double minPathLength = 0.5; // mm
    bool optimizePaths = true;
};

class GCodeExporter {
public:
    static bool exportGCode(const QString& filepath,
                            const QVector<DrawingGeometry>& geometries,
                            const DrawingAreaConfig& drawingArea,
                            int imageWidth,
                            int imageHeight,
                            const GCodeSettings& settings = GCodeSettings());
};
