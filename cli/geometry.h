#pragma once
/**
 * geometry.h — Core geometry primitives, path operations, and curve generators.
 * Header-only C++ port of app/core/geometry.py
 */

#define _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <utility>
#include <algorithm>
#include <numeric>
#include <limits>
#include <cassert>

#include "core_types.h"

// ---------------------------------------------------------------------------
// Basic math helpers
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// Douglas-Peucker path simplification
// ---------------------------------------------------------------------------
namespace detail {
inline double point_to_segment_dist(const Point& p,
                                    const Point& a, const Point& b) {
    double ax = p.first  - a.first;
    double ay = p.second - a.second;
    double bx = b.first  - a.first;
    double by = b.second - a.second;
    double len2 = bx * bx + by * by;
    if (len2 < 1e-12) return std::hypot(ax, ay);
    double t = std::max(0.0, std::min(1.0, (ax * bx + ay * by) / len2));
    double dx = ax - t * bx;
    double dy = ay - t * by;
    return std::hypot(dx, dy);
}

inline Path dp_recursive(const Path& pts, std::size_t lo, std::size_t hi,
                          double tol) {
    if (hi - lo < 2) {
        return { pts[lo] };
    }
    double maxDist = 0.0;
    std::size_t maxIdx = lo + 1;
    for (std::size_t i = lo + 1; i < hi; ++i) {
        double d = point_to_segment_dist(pts[i], pts[lo], pts[hi]);
        if (d > maxDist) { maxDist = d; maxIdx = i; }
    }
    if (maxDist > tol) {
        auto left  = dp_recursive(pts, lo, maxIdx, tol);
        auto right = dp_recursive(pts, maxIdx, hi, tol);
        left.insert(left.end(), right.begin(), right.end());
        return left;
    }
    return { pts[lo] };
}
} // namespace detail

inline Path simplify_path_dp(const Path& path, double tolerance) {
    if (path.size() <= 2) return path;
    auto result = detail::dp_recursive(path, 0, path.size() - 1, tolerance);
    result.push_back(path.back());
    return result;
}

// ---------------------------------------------------------------------------
// Path utilities
// ---------------------------------------------------------------------------
inline std::vector<Path> filter_short_paths(const std::vector<Path>& paths,
                                             double minLen) {
    std::vector<Path> out;
    for (auto& p : paths)
        if (path_length(p) >= minLen) out.push_back(p);
    return out;
}

inline std::vector<Path> sort_paths_nearest(const std::vector<Path>& paths,
                                             Point start = {0.0, 0.0}) {
    if (paths.empty()) return {};
    std::vector<bool> used(paths.size(), false);
    std::vector<Path> sorted;
    sorted.reserve(paths.size());
    Point current = start;

    for (std::size_t iter = 0; iter < paths.size(); ++iter) {
        double bestDist = std::numeric_limits<double>::infinity();
        int    bestIdx  = -1;
        bool   bestRev  = false;

        for (std::size_t i = 0; i < paths.size(); ++i) {
            if (used[i] || paths[i].empty()) continue;
            double dFront = geom_distance(current, paths[i].front());
            double dBack  = geom_distance(current, paths[i].back());
            if (dFront < bestDist) { bestDist = dFront; bestIdx = (int)i; bestRev = false; }
            if (dBack  < bestDist) { bestDist = dBack;  bestIdx = (int)i; bestRev = true;  }
        }
        if (bestIdx < 0) break;
        used[bestIdx] = true;
        Path p = paths[bestIdx];
        if (bestRev && p.size() > 1) std::reverse(p.begin(), p.end());
        current = p.back();
        sorted.push_back(std::move(p));
    }
    return sorted;
}

// ---------------------------------------------------------------------------
// Coordinate transforms
// ---------------------------------------------------------------------------
inline std::vector<Path> scale_paths(const std::vector<Path>& paths,
                                      double sx, double sy,
                                      double ox = 0.0, double oy = 0.0) {
    std::vector<Path> out;
    out.reserve(paths.size());
    for (auto& path : paths) {
        Path p;
        p.reserve(path.size());
        for (auto& pt : path)
            p.push_back({ pt.first * sx + ox, pt.second * sy + oy });
        out.push_back(std::move(p));
    }
    return out;
}

inline std::vector<Path> translate_paths(const std::vector<Path>& paths,
                                          double dx, double dy) {
    std::vector<Path> out;
    for (auto& path : paths) {
        Path p;
        p.reserve(path.size());
        for (auto& pt : path) p.push_back({ pt.first + dx, pt.second + dy });
        out.push_back(std::move(p));
    }
    return out;
}

// ---------------------------------------------------------------------------
// Path Merging — joins paths whose endpoints are within mergeTolerance
// ---------------------------------------------------------------------------
/**
 * Greedily concatenates paths whose endpoints fall within `mergeTolerance`
 * of one another, eliminating unnecessary pen-up/pen-down cycles.
 *
 * Algorithm:
 *  1. Pick first unused path as the "current" chain.
 *  2. Scan all remaining paths for the closest endpoint (considering reversal).
 *  3. If distance <= mergeTolerance, append it (reversing if needed); otherwise
 *     close the chain and start a new one.
 *  4. Repeat until all paths are consumed.
 */
inline std::vector<Path> merge_adjacent_paths(const std::vector<Path>& paths,
                                               double mergeTolerance) {
    if (paths.empty() || mergeTolerance <= 0.0) return paths;

    std::vector<bool> used(paths.size(), false);
    std::vector<Path> merged;
    merged.reserve(paths.size());

    for (std::size_t seed = 0; seed < paths.size(); ++seed) {
        if (used[seed] || paths[seed].empty()) continue;

        Path chain = paths[seed];
        used[seed] = true;

        bool extended = true;
        while (extended) {
            extended = false;
            double bestDist = mergeTolerance;
            int    bestIdx  = -1;
            bool   bestRev  = false;

            const Point& tail = chain.back();
            for (std::size_t i = 0; i < paths.size(); ++i) {
                if (used[i] || paths[i].empty()) continue;
                double dFront = geom_distance(tail, paths[i].front());
                double dBack  = geom_distance(tail, paths[i].back());
                if (dFront <= bestDist) { bestDist = dFront; bestIdx = (int)i; bestRev = false; }
                if (dBack  <= bestDist) { bestDist = dBack;  bestIdx = (int)i; bestRev = true;  }
            }

            if (bestIdx >= 0) {
                used[bestIdx] = true;
                const Path& ext = paths[bestIdx];
                if (bestRev) {
                    chain.insert(chain.end(), ext.rbegin(), ext.rend());
                } else {
                    chain.insert(chain.end(), ext.begin(), ext.end());
                }
                extended = true;
            }
        }

        merged.push_back(std::move(chain));
    }
    return merged;
}

// ---------------------------------------------------------------------------
