#pragma once
/**
 * geometry.h — Core geometry primitives, path operations, and curve generators.
 * Header-only C++ port of app/core/geometry.py
 */

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <utility>
#include <algorithm>
#include <numeric>
#include <limits>
#include <cassert>

// ---------------------------------------------------------------------------
// Type aliases
// ---------------------------------------------------------------------------
using Point = std::pair<double, double>;
using Path  = std::vector<Point>;

// ---------------------------------------------------------------------------
// Basic math helpers
// ---------------------------------------------------------------------------
inline double geom_distance(const Point& a, const Point& b) {
    double dx = b.first  - a.first;
    double dy = b.second - a.second;
    return std::hypot(dx, dy);
}

inline double path_length(const Path& path) {
    double total = 0.0;
    for (std::size_t i = 1; i < path.size(); ++i)
        total += geom_distance(path[i - 1], path[i]);
    return total;
}

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
// Curve generators
// ---------------------------------------------------------------------------

// Quadratic Bézier
inline Point quad_bezier_point(const Point& p0, const Point& p1, const Point& p2,
                                double t) {
    double u = 1.0 - t;
    return { u*u*p0.first  + 2*u*t*p1.first  + t*t*p2.first,
             u*u*p0.second + 2*u*t*p1.second + t*t*p2.second };
}

inline Path quad_bezier_path(const Point& p0, const Point& p1, const Point& p2,
                              int segments = 20) {
    Path result;
    result.reserve(segments + 1);
    for (int i = 0; i <= segments; ++i)
        result.push_back(quad_bezier_point(p0, p1, p2, (double)i / segments));
    return result;
}

// Cubic Bézier
inline Point cubic_bezier_point(const Point& p0, const Point& p1,
                                 const Point& p2, const Point& p3, double t) {
    double u = 1.0 - t;
    return { u*u*u*p0.first  + 3*u*u*t*p1.first  + 3*u*t*t*p2.first  + t*t*t*p3.first,
             u*u*u*p0.second + 3*u*u*t*p1.second + 3*u*t*t*p2.second + t*t*t*p3.second };
}

inline Path cubic_bezier_path(const Point& p0, const Point& p1,
                               const Point& p2, const Point& p3, int segments = 20) {
    Path result;
    result.reserve(segments + 1);
    for (int i = 0; i <= segments; ++i)
        result.push_back(cubic_bezier_point(p0, p1, p2, p3, (double)i / segments));
    return result;
}

// Catmull-Rom spline
inline Point catmull_rom_point(const Point& p0, const Point& p1,
                                const Point& p2, const Point& p3,
                                double t, double alpha = 0.5) {
    auto tj = [&](double ti, const Point& pi, const Point& pj) {
        double dx = pj.first - pi.first, dy = pj.second - pi.second;
        double l = std::sqrt(dx*dx + dy*dy);
        return ti + std::max(l, 1e-10) * alpha;   // Note: alpha power for centripetal
    };
    // Actually: raise to alpha power
    auto tjA = [&](double ti, const Point& pi, const Point& pj) {
        double dx = pj.first - pi.first, dy = pj.second - pi.second;
        double l = std::sqrt(dx*dx + dy*dy);
        return ti + std::pow(std::max(l, 1e-10), alpha);
    };

    double t0 = 0.0;
    double t1 = tjA(t0, p0, p1);
    double t2 = tjA(t1, p1, p2);
    double t3 = tjA(t2, p2, p3);
    double tv = t1 + t * (t2 - t1);

    auto lerp = [](const Point& a, const Point& b, double ta, double tb, double tc) -> Point {
        if (std::abs(tb - ta) < 1e-10) return a;
        double f = (tc - ta) / (tb - ta);
        return { a.first + f*(b.first - a.first), a.second + f*(b.second - a.second) };
    };

    auto a1 = lerp(p0, p1, t0, t1, tv);
    auto a2 = lerp(p1, p2, t1, t2, tv);
    auto a3 = lerp(p2, p3, t2, t3, tv);
    auto b1 = lerp(a1, a2, t0, t2, tv);
    auto b2 = lerp(a2, a3, t1, t3, tv);
    return lerp(b1, b2, t1, t2, tv);
}

inline Path catmull_rom_chain(const std::vector<Point>& pts,
                               int segmentsPerSpan = 20, double alpha = 0.5) {
    if (pts.size() < 2) return pts;
    // Pad endpoints
    std::vector<Point> p = { pts.front() };
    p.insert(p.end(), pts.begin(), pts.end());
    p.push_back(pts.back());

    Path result;
    for (std::size_t i = 1; i < p.size() - 2; ++i) {
        for (int s = 0; s < segmentsPerSpan; ++s) {
            double t = (double)s / segmentsPerSpan;
            result.push_back(catmull_rom_point(p[i-1], p[i], p[i+1], p[i+2], t, alpha));
        }
    }
    result.push_back(pts.back());
    return result;
}

// ---------------------------------------------------------------------------
// Shape generators
// ---------------------------------------------------------------------------
inline Path generate_circle(double cx, double cy, double radius, int segments = 16) {
    Path path;
    path.reserve(segments + 1);
    for (int i = 0; i <= segments; ++i) {
        double angle = (double)i / segments * M_PI * 2.0;
        path.push_back({ cx + std::cos(angle) * radius,
                         cy + std::sin(angle) * radius });
    }
    return path;
}

inline Path generate_ellipse(double cx, double cy, double rx, double ry,
                              double rotationDeg = 0.0, int segments = 36) {
    double cosR = std::cos(rotationDeg * M_PI / 180.0);
    double sinR = std::sin(rotationDeg * M_PI / 180.0);
    Path path;
    path.reserve(segments + 1);
    for (int i = 0; i <= segments; ++i) {
        double angle = 2.0 * M_PI * i / segments;
        double x = rx * std::cos(angle);
        double y = ry * std::sin(angle);
        path.push_back({ cx + x * cosR - y * sinR,
                         cy + x * sinR + y * cosR });
    }
    return path;
}

inline Path generate_rectangle(double cx, double cy, double w, double h,
                                double rotationDeg = 0.0) {
    double hw = w / 2.0, hh = h / 2.0;
    double cosR = std::cos(rotationDeg * M_PI / 180.0);
    double sinR = std::sin(rotationDeg * M_PI / 180.0);
    std::vector<std::pair<double,double>> corners = {
        {-hw,-hh},{hw,-hh},{hw,hh},{-hw,hh},{-hw,-hh}
    };
    Path path;
    for (auto& c : corners)
        path.push_back({ cx + c.first * cosR - c.second * sinR,
                         cy + c.first * sinR + c.second * cosR });
    return path;
}

inline Path generate_polygon(double cx, double cy, double radius, int sides,
                              double angleOffsetDeg = 0.0) {
    Path path;
    double offset = angleOffsetDeg * M_PI / 180.0;
    for (int i = 0; i <= sides; ++i) {
        double angle = offset + (double)i / sides * M_PI * 2.0;
        path.push_back({ cx + std::cos(angle) * radius,
                         cy + std::sin(angle) * radius });
    }
    return path;
}

inline Path generate_star(double cx, double cy, double rOuter,
                           double rInner = -1.0, int points = 5,
                           double rotationDeg = 0.0) {
    if (rInner < 0.0) rInner = rOuter * 0.4;
    int total = points * 2;
    double rotRad = rotationDeg * M_PI / 180.0 - M_PI / 2.0;
    Path path;
    for (int i = 0; i <= total; ++i) {
        double angle = rotRad + 2.0 * M_PI * i / total;
        double r = (i % 2 == 0) ? rOuter : rInner;
        path.push_back({ cx + r * std::cos(angle), cy + r * std::sin(angle) });
    }
    return path;
}
