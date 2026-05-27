#include "geometry.h"
#pragma once
/**
 * gcode_exporter.h — Export path geometries to standard G-code for CNC/plotters.
 */




#include "core_types.h"
#include "coordinates.h"

struct GCodeSettings {
    std::string startGcode = "G21\nG90";
    std::string endGcode = "M30";
    std::string penDownCmd = "M3 S90";
    std::string penUpCmd = "M5";
    std::string startLayerCmd = "";
    std::string endLayerCmd = "";
    double xOffset = 0.0;
    double yOffset = 0.0;
    bool centerZero = false;
    double minPathLength = 0.5; // mm
    bool optimizePaths = true;
    double simplifyTolerance = 0.0;
    double mergeTolerance = 0.0;
    int multipass = 1;
};

class GCodeExporter {
public:
    static bool exportGCode(const std::string& filepath,
                            const std::vector<DrawingGeometry>& geometries,
                            const DrawingAreaConfig& drawingArea,
                            int imageWidth,
                            int imageHeight,
                            const GCodeSettings& settings = GCodeSettings());
};
