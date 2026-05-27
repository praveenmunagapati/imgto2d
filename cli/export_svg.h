#pragma once
#include "core_types.h"
#include "coordinates.h"

void export_svg(const std::string& filename,
                const std::vector<DrawingGeometry>& geometries,
                const DrawingAreaConfig& drawingArea,
                int imageWidth,
                int imageHeight,
                const std::vector<std::string>& penColorsHex,
                double penWidthMm);
