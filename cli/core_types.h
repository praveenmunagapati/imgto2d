#pragma once
#include <iostream>
#include <string>
#include <vector>

#include <map>
#include <variant>
#include <cmath>
#include <random>
#include <fstream>
#include <algorithm>
#include <functional>
#include <queue>
#include <set>
#include <numeric>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <omp.h>

inline float get_pixel_float(const cv::Mat& image, int y, int x) {
    if (image.type() == CV_32F) return image.at<float>(y, x);
    if (image.type() == CV_8U) return (float)image.at<uchar>(y, x);
    return 0.0f;
}

// ===========================================================================
// Core Data Types & Geometry
// ===========================================================================

using Point = std::pair<double, double>;
using Path  = std::vector<Point>;

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

inline float randUniform(float min, float max) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

inline Path generate_circle(double cx, double cy, double radius, int segments = 16) {
    Path path;
    path.reserve(segments + 1);
    for (int i = 0; i <= segments; ++i) {
        double angle = (double)i / segments * 3.141592653589793 * 2.0;
        path.push_back({ cx + std::cos(angle) * radius,
                         cy + std::sin(angle) * radius });
    }
    return path;
}

inline Path generate_polygon(double cx, double cy, double radius, int sides,
                              double angleOffsetDeg = 0.0) {
    Path path;
    double offset = angleOffsetDeg * 3.141592653589793 / 180.0;
    for (int i = 0; i <= sides; ++i) {
        double angle = offset + (double)i / sides * 3.141592653589793 * 2.0;
        path.push_back({ cx + std::cos(angle) * radius,
                         cy + std::sin(angle) * radius });
    }
    return path;
}

inline Path generate_rectangle(double cx, double cy, double w, double h, double rotationDeg = 0.0) {
    double hw = w / 2.0, hh = h / 2.0;
    double cosR = std::cos(rotationDeg * 3.141592653589793 / 180.0);
    double sinR = std::sin(rotationDeg * 3.141592653589793 / 180.0);
    std::vector<std::pair<double,double>> corners = {{-hw,-hh},{hw,-hh},{hw,hh},{-hw,hh},{-hw,-hh}};
    Path path;
    for (auto& c : corners)
        path.push_back({ cx + c.first * cosR - c.second * sinR,
                         cy + c.first * sinR + c.second * cosR });
    return path;
}

inline Path generate_star(double cx, double cy, double rOuter, double rInner = -1.0, int points = 5, double rotationDeg = 0.0) {
    if (rInner < 0.0) rInner = rOuter * 0.4;
    int total = points * 2;
    double rotRad = rotationDeg * 3.141592653589793 / 180.0 - 3.141592653589793 / 2.0;
    Path path;
    for (int i = 0; i <= total; ++i) {
        double angle = rotRad + 2.0 * 3.141592653589793 * i / total;
        double r = (i % 2 == 0) ? rOuter : rInner;
        path.push_back({ cx + r * std::cos(angle), cy + r * std::sin(angle) });
    }
    return path;
}

inline Path generate_ellipse(double cx, double cy, double rx, double ry, double rotationDeg = 0.0, int segments = 36) {
    double cosR = std::cos(rotationDeg * 3.141592653589793 / 180.0);
    double sinR = std::sin(rotationDeg * 3.141592653589793 / 180.0);
    Path path;
    for (int i = 0; i <= segments; ++i) {
        double angle = 2.0 * 3.141592653589793 * i / segments;
        double x = rx * std::cos(angle), y = ry * std::sin(angle);
        path.push_back({ cx + x * cosR - y * sinR, cy + x * sinR + y * cosR });
    }
    return path;
}

inline Point quad_bezier_point(const Point& p0, const Point& p1, const Point& p2, double t) {
    double u = 1.0 - t;
    return { u*u*p0.first + 2*u*t*p1.first + t*t*p2.first,
             u*u*p0.second + 2*u*t*p1.second + t*t*p2.second };
}

inline Path quad_bezier_path(const Point& p0, const Point& p1, const Point& p2, int segments = 20) {
    Path result;
    for (int i = 0; i <= segments; ++i)
        result.push_back(quad_bezier_point(p0, p1, p2, (double)i / segments));
    return result;
}

inline Point cubic_bezier_point(const Point& p0, const Point& p1, const Point& p2, const Point& p3, double t) {
    double u = 1.0 - t;
    return { u*u*u*p0.first + 3*u*u*t*p1.first + 3*u*t*t*p2.first + t*t*t*p3.first,
             u*u*u*p0.second + 3*u*u*t*p1.second + 3*u*t*t*p2.second + t*t*t*p3.second };
}

inline Path cubic_bezier_path(const Point& p0, const Point& p1, const Point& p2, const Point& p3, int segments = 20) {
    Path result;
    for (int i = 0; i <= segments; ++i)
        result.push_back(cubic_bezier_point(p0, p1, p2, p3, (double)i / segments));
    return result;
}

inline Point catmull_rom_point(const Point& p0, const Point& p1, const Point& p2, const Point& p3, double t, double alpha = 0.5) {
    auto tjA = [&](double ti, const Point& pi, const Point& pj) {
        double dx = pj.first - pi.first, dy = pj.second - pi.second;
        double l = std::sqrt(dx*dx + dy*dy);
        return ti + std::pow(std::max(l, 1e-10), alpha);
    };
    double t0 = 0.0, t1 = tjA(0.0, p0, p1), t2 = tjA(t1, p1, p2), t3 = tjA(t2, p2, p3);
    double tv = t1 + t * (t2 - t1);
    auto lerp = [](const Point& a, const Point& b, double ta, double tb, double tc) -> Point {
        if (std::abs(tb - ta) < 1e-10) return a;
        double f = (tc - ta) / (tb - ta);
        return { a.first + f*(b.first - a.first), a.second + f*(b.second - a.second) };
    };
    auto a1 = lerp(p0, p1, t0, t1, tv); auto a2 = lerp(p1, p2, t1, t2, tv); auto a3 = lerp(p2, p3, t2, t3, tv);
    auto b1 = lerp(a1, a2, t0, t2, tv); auto b2 = lerp(a2, a3, t1, t3, tv);
    return lerp(b1, b2, t1, t2, tv);
}

inline Path catmull_rom_chain(const std::vector<Point>& pts, int segmentsPerSpan = 20, double alpha = 0.5) {
    if (pts.size() < 2) return pts;
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

struct DrawingGeometry {
    Path path;
    int  penIndex   = 0;
    int  groupIndex = 0;
};

// ===========================================================================
// Settings System (Replacing QVariant)
// ===========================================================================

enum class SettingType {
    Number,
    Integer,
    Percentage,
    Boolean,
    Enum,
    Text,
};

struct SettingValue {
    std::variant<std::monostate, int, double, bool, std::string> val;

    SettingValue() : val(std::monostate{}) {}
    SettingValue(int v) : val(v) {}
    SettingValue(double v) : val(v) {}
    SettingValue(bool v) : val(v) {}
    SettingValue(const std::string& v) : val(v) {}
    SettingValue(const char* v) : val(std::string(v)) {}

    bool isNull() const { return std::holds_alternative<std::monostate>(val); }

    int toInt() const {
        if (auto p = std::get_if<int>(&val)) return *p;
        if (auto p = std::get_if<double>(&val)) return static_cast<int>(*p);
        return 0;
    }

    double toDouble() const {
        if (auto p = std::get_if<double>(&val)) return *p;
        if (auto p = std::get_if<int>(&val)) return static_cast<double>(*p);
        return 0.0;
    }

    bool toBool() const {
        if (auto p = std::get_if<bool>(&val)) return *p;
        return false;
    }

    std::string toString() const {
        if (auto p = std::get_if<std::string>(&val)) return *p;
        return "";
    }
};

struct PFMSetting {
    std::string     key;
    std::string     label;
    SettingType     type        = SettingType::Number;
    SettingValue    defaultVal;
    SettingValue    value;          // null = use default
    double          minVal      = 0.0;
    double          maxVal      = 100.0;
    double          safeMin     = 0.0;
    double          safeMax     = 100.0;
    double          step        = 1.0;
    std::vector<std::string> options;
    std::string     category    = "Default";
    std::string     tooltip;
    bool            randomiseExclude = false;

    SettingValue currentValue() const {
        return value.isNull() ? defaultVal : value;
    }

    double toDouble() const { return currentValue().toDouble(); }
    int    toInt()    const { return currentValue().toInt(); }
    bool   toBool()   const { return currentValue().toBool(); }
};

// ===========================================================================
