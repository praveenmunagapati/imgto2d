#include "coordinates.h"

void compute_scaling(double srcW, double srcH, double dstW, double dstH, ScalingMode mode,
                     double& sx, double& sy, double& ox, double& oy) {
    if (mode == ScalingMode::Stretch) {
        sx = (srcW > 0) ? dstW / srcW : 1.0;
        sy = (srcH > 0) ? dstH / srcH : 1.0;
        ox = 0.0;
        oy = 0.0;
        return;
    }

    double srcAspect = (srcH > 0) ? srcW / srcH : 1.0;
    double dstAspect = (dstH > 0) ? dstW / dstH : 1.0;

    if (mode == ScalingMode::Fit) {
        if (srcAspect > dstAspect) {
            sx = sy = (srcW > 0) ? dstW / srcW : 1.0;
            ox = 0.0;
            oy = (dstH - srcH * sy) / 2.0;
        } else {
            sx = sy = (srcH > 0) ? dstH / srcH : 1.0;
            ox = (dstW - srcW * sx) / 2.0;
            oy = 0.0;
        }
    } else if (mode == ScalingMode::Crop) {
        if (srcAspect > dstAspect) {
            sx = sy = (srcH > 0) ? dstH / srcH : 1.0;
            ox = (dstW - srcW * sx) / 2.0;
            oy = 0.0;
        } else {
            sx = sy = (srcW > 0) ? dstW / srcW : 1.0;
            ox = 0.0;
            oy = (dstH - srcH * sy) / 2.0;
        }
    } else {
        sx = sy = 1.0;
        ox = oy = 0.0;
    }
}

Path pixel_path_to_mm(const Path& path, double imgW, double imgH, const DrawingAreaConfig& config) {
    if (path.empty() || imgW <= 0 || imgH <= 0) return path;

    double dw = config.usableWidth();
    double dh = config.usableHeight();
    if (dw <= 0 || dh <= 0) return path;

    double sx, sy, ox, oy;
    compute_scaling(imgW, imgH, dw, dh, config.scaling_mode, sx, sy, ox, oy);

    double pl = config.padding_left_mm;
    double pt = config.padding_top_mm;

    Path scaled;
    scaled.reserve(path.size());
    for (const auto& pt_orig : path) {
        double nx = pt_orig.first * sx + ox + pl;
        double ny = pt_orig.second * sy + oy + pt;
        scaled.push_back({nx, ny});
    }

    return scaled;
}
