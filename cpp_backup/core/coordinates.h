#pragma once
/**
 * coordinates.h — Coordinate transforms and mm conversions.
 */

#include "core/geometry.h"

// Equivalent to python ScalingMode
enum class ScalingMode {
    Fit,
    Crop,
    Stretch
};

// Represents the DrawingArea setup from the UI
struct DrawingAreaConfig {
    double width_mm = 210.0;
    double height_mm = 297.0;
    double padding_left_mm = 0.0;
    double padding_top_mm = 0.0;
    double padding_right_mm = 0.0;
    double padding_bottom_mm = 0.0;
    ScalingMode scaling_mode = ScalingMode::Fit;

    double usableWidth() const {
        return std::max(0.0, width_mm - padding_left_mm - padding_right_mm);
    }
    double usableHeight() const {
        return std::max(0.0, height_mm - padding_top_mm - padding_bottom_mm);
    }
};

// Computes the sx, sy, ox, oy parameters for mapping
void compute_scaling(double srcW, double srcH, double dstW, double dstH, ScalingMode mode,
                     double& sx, double& sy, double& ox, double& oy);

// Converts a path from raw image pixel coordinates into physical mm paper coordinates
Path pixel_path_to_mm(const Path& path, double imgW, double imgH, const DrawingAreaConfig& config);
