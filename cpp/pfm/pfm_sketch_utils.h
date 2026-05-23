#pragma once
#include "pfm/pfm_base.h"

/**
 * pfm_sketch_utils.h — Shared utilities for sketch-style PFMs.
 * Provides the common darkest-area loop and erase-line logic used by
 * SketchLines, SketchCurves, SketchBeziers (all share the same core).
 */

#include <cmath>
#include <opencv2/imgproc.hpp>
#include "core/geometry.h"

namespace sketch_utils {

// ---------------------------------------------------------------------------
// Erase a line segment on a float image (identical logic across all sketch PFMs)
// ---------------------------------------------------------------------------
inline void eraseLine(cv::Mat& img,
                      double x0, double y0, double x1, double y1,
                      double eraseMin, double eraseMax,
                      double radiusMin, double radiusMax,
                      double tone, int W, int H)
{
    double dist = std::hypot(x1 - x0, y1 - y0);
    int steps = std::max(1, (int)dist);
    float* data = img.ptr<float>(0);

    for (int s = 0; s <= steps; ++s) {
        double t       = (double)s / steps;
        double toneT   = (tone > 0.0) ? std::pow(t, 1.0 / std::max(0.01, tone)) : 0.5;
        double eraseVal = eraseMin + (eraseMax - eraseMin) * toneT;
        double radius   = radiusMin + (radiusMax - radiusMin) * toneT;

        double px = x0 + (x1 - x0) * t;
        double py = y0 + (y1 - y0) * t;
        int pxi = (int)px, pyi = (int)py;
        int rInt = std::max(0, (int)radius);

        if (rInt == 0) {
            if (pxi >= 0 && pxi < W && pyi >= 0 && pyi < H)
                data[pyi * W + pxi] = std::min(255.0f, data[pyi * W + pxi] + (float)eraseVal);
        } else {
            int yLo = std::max(0, pyi - rInt), yHi = std::min(H, pyi + rInt + 1);
            int xLo = std::max(0, pxi - rInt), xHi = std::min(W, pxi + rInt + 1);
            for (int ey = yLo; ey < yHi; ++ey)
                for (int ex = xLo; ex < xHi; ++ex)
                    if (std::hypot(ex - px, ey - py) <= radius)
                        data[ey * W + ex] = std::min(255.0f, data[ey * W + ex] + (float)eraseVal);
        }
    }
}

// ---------------------------------------------------------------------------
// Erase a rectangular region (used by Squares and Shapes)
// ---------------------------------------------------------------------------
inline void eraseRect(cv::Mat& img,
                      int x0, int y0, int x1, int y1,
                      double eraseVal, int W, int H)
{
    x0 = std::max(0, x0); y0 = std::max(0, y0);
    x1 = std::min(W - 1, x1); y1 = std::min(H - 1, y1);
    if (x1 < x0 || y1 < y0) return;
    float* data = img.ptr<float>(0);
    for (int ey = y0; ey <= y1; ++ey)
        for (int ex = x0; ex <= x1; ++ex)
            data[ey * W + ex] = std::min(255.0f, data[ey * W + ex] + (float)eraseVal);
}

// ---------------------------------------------------------------------------
// Build edge/sobel/direction maps
// ---------------------------------------------------------------------------
struct EdgeMaps {
    cv::Mat edge;       // float [0..1]
    cv::Mat sobel;      // float [0..1]
    cv::Mat direction;  // float atan2 radians
};

inline EdgeMaps buildEdgeMaps(const cv::Mat& workImg,
                               double edgePower, double sobelPower, double directionality)
{
    EdgeMaps m;
    if (edgePower > 0) {
        cv::Mat edges;
        cv::Canny(workImg, edges, 50, 150);
        edges.convertTo(m.edge, CV_32F, 1.0 / 255.0);
    }
    if (sobelPower > 0 || directionality > 0) {
        cv::Mat sx, sy;
        cv::Sobel(workImg, sx, CV_64F, 1, 0, 3);
        cv::Sobel(workImg, sy, CV_64F, 0, 1, 3);
        if (sobelPower > 0) {
            cv::Mat mag;
            cv::magnitude(sx, sy, mag);
            double mn, mx;
            cv::minMaxLoc(mag, &mn, &mx);
            mag.convertTo(m.sobel, CV_32F, 1.0 / (mx + 1e-10));
        }
        if (directionality > 0) {
            int H = workImg.rows, W = workImg.cols;
            m.direction.create(H, W, CV_32F);
            for (int y = 0; y < H; ++y)
                for (int x = 0; x < W; ++x)
                    m.direction.at<float>(y, x) =
                        (float)std::atan2(sy.at<double>(y, x), sx.at<double>(y, x));
        }
    }
    return m;
}

// ---------------------------------------------------------------------------
// Compute per-iteration density progress
// ---------------------------------------------------------------------------
inline double densityProgress(const cv::Mat& lightened,
                               double initialBrightness, double targetBrightness) {
    cv::Scalar m = cv::mean(lightened);
    double cur = m[0];
    double prog = (cur - initialBrightness) / std::max(1.0, targetBrightness - initialBrightness);
    return std::clamp(prog, 0.0, 1.0);
}

} // namespace sketch_utils
