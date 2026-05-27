#include "geometry.h"
#pragma once
/**
 * hpgl_exporter.h — Export path geometries to HPGL format for legacy pen plotters.
 */



#include "core_types.h"
#include "coordinates.h"

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
    double simplifyTolerance = 0.0;
    int rotation = 0; // 0, 90, 180, 270 degrees
    int penForce = -1; // -1 means disabled, otherwise FS command (Force Select)
};

class HPGLExporter {
public:
    static bool exportHPGL(const std::string& filepath,
                           const std::vector<DrawingGeometry>& geometries,
                           const DrawingAreaConfig& drawingArea,
                           int imageWidth,
                           int imageHeight,
                           const HPGLSettings& settings = HPGLSettings());
};
