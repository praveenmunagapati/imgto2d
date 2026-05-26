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
// Filters Architecture
// ===========================================================================

class ImageFilter {
public:
    virtual ~ImageFilter() = default;

    virtual std::string name() const = 0;
    virtual std::string category() const { return "General"; }
    virtual std::string description() const { return ""; }

    SettingValue get(const std::string& key) const {
        auto it = m_settings.find(key);
        if (it != m_settings.end()) return it->second.currentValue();
        return SettingValue();
    }

    void set(const std::string& key, const SettingValue& value) {
        if (m_settings.count(key)) m_settings[key].value = value;
    }

    virtual cv::Mat process(const cv::Mat& image) = 0;

protected:
    virtual std::vector<PFMSetting> defineSettings() const { return {}; }
    
    void initSettings() {
        auto list = defineSettings();
        for (const auto& s : list) {
            m_settings[s.key] = s;
        }
    }
    std::map<std::string, PFMSetting> m_settings;
};

// ---------------------------------------------------------------------------
// Threshold Filter Implementation
// ---------------------------------------------------------------------------
class ThresholdFilter : public ImageFilter {
public:
    ThresholdFilter() { initSettings(); }
    std::string name() const override { return "Threshold"; }

protected:
    std::vector<PFMSetting> defineSettings() const override {
        return {
            { "threshold", "Threshold", SettingType::Integer, 127, SettingValue(), 0.0, 255.0, 0.0, 255.0, 1.0 }
        };
    }

public:
    cv::Mat process(const cv::Mat& image) override {
        int t = get("threshold").toInt();
        cv::Mat out;
        cv::threshold(image, out, t, 255, cv::THRESH_BINARY);
        return out;
    }
};

// ===========================================================================
// Path Finding Modules Architecture
// ===========================================================================

class PathFindingModule {
public:
    virtual ~PathFindingModule() = default;

    virtual std::string name() const = 0;
    virtual std::string description() const { return ""; }
    virtual std::string category() const { return "General"; }
    virtual bool isPremium() const { return false; }

    SettingValue get(const std::string& key) const {
        auto it = m_settings.find(key);
        if (it != m_settings.end()) return it->second.currentValue();
        return SettingValue();
    }

    void set(const std::string& key, const SettingValue& value) {
        if (m_settings.count(key)) m_settings[key].value = value;
    }

    std::vector<DrawingGeometry> process(const cv::Mat& grayImage) {
        return _process(grayImage);
    }

    void cancel() { m_cancelled = true; }
    bool isCancelled() const { return m_cancelled; }

    int weightedChoice(const std::vector<double>& weights) {
        std::discrete_distribution<int> dist(weights.begin(), weights.end());
        return dist(m_rng);
    }

    double randUniform(double lo, double hi) {
        std::uniform_real_distribution<double> dist(lo, hi);
        return dist(m_rng);
    }

    double randGauss(double mean, double sigma) {
        std::normal_distribution<double> dist(mean, sigma);
        return dist(m_rng);
    }

    int randInt(int lo, int hi) {
        std::uniform_int_distribution<int> dist(lo, hi);
        return dist(m_rng);
    }

    void emitProgress(float pct, int shapes, const std::string& text) {
        std::cout << "\rProgress: " << int(pct * 100) << "% - " << text << std::flush;
    }

    std::vector<PFMSetting> settingsList() const {
        std::vector<PFMSetting> out;
        for (const auto& kv : m_settings) out.push_back(kv.second);
        return out;
    }

protected:
    virtual std::vector<PFMSetting> defineSettings() const = 0;
    virtual std::vector<DrawingGeometry> _process(const cv::Mat& image) = 0;

    void initSettings() {
        m_rng.seed(42);
        auto list = defineSettings();
        for (const auto& s : list) {
            m_settings[s.key] = s;
        }
    }

    std::map<std::string, PFMSetting> m_settings;
    std::mt19937 m_rng;
    bool m_cancelled = false;
};

// ---------------------------------------------------------------------------
// Nearest Neighbor TSP Solver (from base_tsppfm.h)
// ---------------------------------------------------------------------------
static inline Path solve_tsp_nn(const std::vector<cv::Point2f>& points,
                                std::function<bool()> isCancelledFunc = []{ return false; },
                                std::function<void(float)> progressFunc = [](float){}) {
    if (points.empty()) return {};
    
    std::vector<bool> visited(points.size(), false);
    Path path;
    path.reserve(points.size());
    
    int current = 0;
    visited[0] = true;
    path.push_back({points[0].x, points[0].y});
    
    int remaining = points.size() - 1;
    int total = remaining;

    while (remaining > 0) {
        if (isCancelledFunc()) return {};
        if (remaining % 1000 == 0) progressFunc(1.0f - (float(remaining) / std::max(1, total)));

        int best_i = -1;
        float best_d = 1e12f;
        float cx = points[current].x;
        float cy = points[current].y;
        
        for (size_t i = 0; i < points.size(); ++i) {
            if (!visited[i]) {
                float dx = points[i].x - cx;
                float dy = points[i].y - cy;
                float d = dx*dx + dy*dy;
                if (d < best_d) {
                    best_d = d;
                    best_i = i;
                }
            }
        }
        if (best_i == -1) break;
        
        visited[best_i] = true;
        path.push_back({points[best_i].x, points[best_i].y});
        current = best_i;
        remaining--;
    }
    return path;
}

// ---------------------------------------------------------------------------
// Base TSP PFM Implementation
// ---------------------------------------------------------------------------
class BaseTSPPFM : public PathFindingModule {
public:
    BaseTSPPFM() { initSettings(); }
    bool isPremium() const override { return true; }

protected:
    std::vector<PFMSetting> defineSettings() const override {
        return {
            {"nodes", "Nodes", SettingType::Integer, 2000, SettingValue(), 100.0, 20000.0, 100.0, 20000.0, 100.0}
        };
    }

    virtual std::vector<double> getProbabilities(const cv::Mat& image) = 0;

    std::vector<DrawingGeometry> _process(const cv::Mat& image) override {
        int w = image.cols;
        int h = image.rows;
        int nodes = get("nodes").toInt();

        std::vector<double> probs = getProbabilities(image);
        if (probs.empty()) return {};
        
        double sum = 0.0;
        for (auto p : probs) sum += p;
        if (sum < 1e-6) return {};
        for (auto& p : probs) p /= sum;

        std::cout << "Selecting " << nodes << " points..." << std::endl;
        std::vector<cv::Point2f> points;
        for (int i = 0; i < nodes; ++i) {
            if (isCancelled()) return {};
            int idx = weightedChoice(probs);
            points.push_back(cv::Point2f(idx % w, idx / w));
        }

        auto path = solve_tsp_nn(points, 
            [this](){ return isCancelled(); }, 
            [this](float p){ emitProgress(0.5f + p * 0.5f, 0, "Solving TSP..."); }
        );
        std::cout << std::endl;

        if (path.size() >= 2) {
            DrawingGeometry dg;
            dg.path = path;
            return {dg};
        }
        return {};
    }
};

// ---------------------------------------------------------------------------
// TSP Classic Implementation
// ---------------------------------------------------------------------------
class TSPClassicPFM : public BaseTSPPFM {
public:
    TSPClassicPFM() : BaseTSPPFM() {} // BaseTSPPFM constructor calls initSettings
    std::string name() const override { return "TSP Classic"; }

protected:
    std::vector<double> getProbabilities(const cv::Mat& image) override {
        std::vector<double> probs(image.cols * image.rows, 0.0);
        for (int y = 0; y < image.rows; ++y) {
            const uchar* row = image.ptr<uchar>(y);
            for (int x = 0; x < image.cols; ++x) {
                double d = 255.0 - row[x];
                if (d > 0) probs[y * image.cols + x] = d;
            }
        }
        return probs;
    }
};

// ===========================================================================








// ===========================================================================
// AUTO-PORTED PFM ALGORITHMS (all in namespace pfm_ported)
// ===========================================================================
namespace pfm_ported {

// --- Shared utility: nearest_seed_radius ---
inline float nearest_seed_radius(float cx, float cy, const std::vector<cv::Point2f>& points) {
    float best = 1e12f;
    for (const auto& pt : points) {
        float dx = pt.x - cx, dy = pt.y - cy;
        if (std::abs(dx) < 1e-6f && std::abs(dy) < 1e-6f) continue;
        float d = std::hypot(dx, dy);
        if (d < best) best = d;
    }
    return best < 1e11f ? best * 0.45f : 8.0f;
}

// --- Shared utility: trace_streamline ---
inline Path trace_streamline(const cv::Mat& fx, const cv::Mat& fy, float x, float y, int max_len, float step_size) {
    Path path;
    int w = fx.cols, h = fx.rows;
    for (int i = 0; i < max_len; ++i) {
        int xi = std::clamp(int(x), 0, w-1), yi = std::clamp(int(y), 0, h-1);
        path.push_back({x, y});
        float vx = fx.at<float>(yi, xi), vy = fy.at<float>(yi, xi);
        float mag = std::hypot(vx, vy);
        if (mag < 1e-6f) break;
        x += (vx/mag)*step_size; y += (vy/mag)*step_size;
        if (x < 0 || x >= w || y < 0 || y >= h) break;
    }
    return path;
}

// --- Shared utility: generate_dfs_maze ---
inline Path generate_dfs_maze(int width, int height) {
    cv::Mat maze = cv::Mat::zeros(height, width, CV_8UC1);
    std::vector<std::pair<int,int>> stack;
    stack.push_back({0,0}); maze.at<uint8_t>(0,0) = 1;
    Path path;
    int dx[] = {0,2,0,-2}, dy[] = {2,0,-2,0};
    std::mt19937 rng(42);
    while (!stack.empty()) {
        auto [x, y] = stack.back();
        path.push_back({x*10.0, y*10.0});
        std::vector<std::pair<int,int>> neighbors;
        for (int i = 0; i < 4; ++i) {
            int nx = x+dx[i], ny = y+dy[i];
            if (nx>=0 && nx<width && ny>=0 && ny<height && maze.at<uint8_t>(ny,nx)==0)
                neighbors.push_back({nx,ny});
        }
        if (!neighbors.empty()) {
            std::uniform_int_distribution<int> dist(0, neighbors.size()-1);
            auto [nx, ny] = neighbors[dist(rng)];
            maze.at<uint8_t>(ny,nx) = 1;
            maze.at<uint8_t>(y+(ny-y)/2, x+(nx-x)/2) = 1;
            stack.push_back({nx,ny});
        } else { stack.pop_back(); }
    }
    return path;
}

// --- pfm_sketch_utils.h (inlined) ---
/**
 * pfm_sketch_utils.h — Shared utilities for sketch-style PFMs.
 * Provides the common darkest-area loop and erase-line logic used by
 * SketchLines, SketchCurves, SketchBeziers (all share the same core).
 */

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


// --- _letters_base_pfm.h ---
class _LettersBasePFM : public PathFindingModule {
public:
    explicit _LettersBasePFM();
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    virtual int getLloydIters() const = 0;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- base_adaptive_pfm.h ---
class BaseAdaptivePFM : public PathFindingModule {
public:
    explicit BaseAdaptivePFM();
    std::string category() const override { return "Adaptive"; }
    bool    isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    // Returns brightness-weighted, Lloyd-relaxed seed points
    std::vector<cv::Point2f> getSeeds(const cv::Mat& image);
};


// --- base_grid_pfm.h ---
class BaseGridPFM : public PathFindingModule {
public:
    explicit BaseGridPFM();
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
};


// --- base_maze_pfm.h ---
class BaseMazePFM : public PathFindingModule {
public:
    explicit BaseMazePFM();
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
    virtual std::pair<float, float> transformPoint(float x, float y);
};


// --- base_mosaic_pfm.h ---
class BaseMosaicPFM : public PathFindingModule {
public:
    explicit BaseMosaicPFM();
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
};


// --- base_multi_hatch_pfm.h ---
class BaseMultiHatchPFM : public PathFindingModule {
public:
    explicit BaseMultiHatchPFM();
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
    virtual std::vector<float> getAngles() const = 0;
};


// --- base_stipple_pfm.h ---
class BaseStipplePFM : public PathFindingModule {
public:
    explicit BaseStipplePFM();
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    virtual Path generateShape(float cx, float cy, float r) = 0;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- base_streamline_pfm.h ---
// trace_streamline: already defined above

class BaseStreamlinePFM : public PathFindingModule {
public:
    explicit BaseStreamlinePFM();
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
};


// --- base_voronoi_extra_pfm.h ---
// nearest_seed_radius: already defined above

class BaseVoronoiExtraPFM : public PathFindingModule {
public:
    explicit BaseVoronoiExtraPFM();
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<cv::Point2f> getSeeds(const cv::Mat& image);
};


// --- lbg_pfms.h ---
std::vector<PFMSetting> makeLbgSettings(std::vector<PFMSetting> settings);


// --- adaptive_circular_scribbles_pfm.h ---
class AdaptiveCircularScribblesPFM : public BaseAdaptivePFM {
public:
    explicit AdaptiveCircularScribblesPFM();
    std::string name() const override { return "Adaptive Circular Scribbles"; }
protected:
    std::vector<PFMSetting>      defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- adaptive_dashes_pfm.h ---
class AdaptiveDashesPFM : public BaseAdaptivePFM {
public:
    explicit AdaptiveDashesPFM() {}
    std::string name() const override { return "Adaptive Dashes"; }
protected:
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- adaptive_diagram_pfm.h ---
class AdaptiveDiagramPFM : public BaseAdaptivePFM {
public:
    explicit AdaptiveDiagramPFM() {}
    std::string name() const override { return "Adaptive Diagram"; }
protected:
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- adaptive_pfm.h ---
/**
 * adaptive_pfm.h — Adaptive Stippling PFM (brightness-weighted dot placement).
 * This is the "Adaptive Stippling" class from tk_gui_fast.py's AVAILABLE_PFMS list.
 * It is a simplified C++ implementation: weighted random sampling without
 * the full Lloyd relaxation from the Python pfm_utils (which has heavy scipy deps).
 */
class AdaptiveStipplingPFM : public PathFindingModule {
public:
    explicit AdaptiveStipplingPFM();
    std::string name()        const override { return "Adaptive Stippling"; }
    std::string description() const override { return "Brightness-weighted adaptive stipple placement."; }
    std::string category()    const override { return "Adaptive"; }
    bool    isPremium()   const override { return true; }
protected:
    std::vector<PFMSetting>      defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- adaptive_letters_pfm.h ---
class AdaptiveLettersPFM : public _LettersBasePFM {
public:
    explicit AdaptiveLettersPFM() {}
    std::string name() const override { return "Adaptive Letters"; }
protected:
    int getLloydIters() const override { return 0; }
};


// --- adaptive_shapes_pfm.h ---
class AdaptiveShapesPFM : public BaseAdaptivePFM {
public:
    explicit AdaptiveShapesPFM();
    std::string name() const override { return "Adaptive Shapes"; }
protected:
    std::vector<PFMSetting>      defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- adaptive_tree_pfm.h ---
class AdaptiveTreePFM : public BaseAdaptivePFM {
public:
    explicit AdaptiveTreePFM() {}
    std::string name() const override { return "Adaptive Tree"; }
protected:
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- adaptive_triangulation_pfm.h ---
class AdaptiveTriangulationPFM : public BaseAdaptivePFM {
public:
    explicit AdaptiveTriangulationPFM() {}
    std::string name() const override { return "Adaptive Triangulation"; }
protected:
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- adaptive_tsppfm.h ---
class AdaptiveTSPPFM : public BaseAdaptivePFM {
public:
    explicit AdaptiveTSPPFM() {}
    std::string name() const override { return "Adaptive TSP"; }
protected:
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- ambient_flow_pfm.h ---
class AmbientFlowPFM : public PathFindingModule {
public:
    explicit AmbientFlowPFM();
    std::string name() const override { return "Ambient Flow"; }
    std::string category() const override { return "Special"; }
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- contour_paths_pfm.h ---
class ContourPathsPFM : public PathFindingModule {
public:
    explicit ContourPathsPFM();
    std::string name() const override { return "Contour Paths"; }
    std::string category() const override { return "Special"; }
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- edge_shading_pfm.h ---
class EdgeShadingPFM : public PathFindingModule {
public:
    explicit EdgeShadingPFM();
    std::string name() const override { return "Edge Shading"; }
    std::string category() const override { return "Special"; }
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- grid_dashes_pfm.h ---
class GridDashesPFM : public BaseGridPFM {
public: explicit GridDashesPFM(){} std::string name() const override{return "Grid Dashes";} protected: std::vector<DrawingGeometry> _process(const cv::Mat& image) override; };


// --- grid_letters_pfm.h ---
class GridLettersPFM : public BaseGridPFM {
public: explicit GridLettersPFM(){} std::string name() const override{return "Grid Letters";} protected: std::vector<DrawingGeometry> _process(const cv::Mat& image) override; };


// --- grid_shapes_pfm.h ---
class GridShapesPFM : public BaseGridPFM {
public: explicit GridShapesPFM(){} std::string name() const override{return "Grid Shapes";} protected: std::vector<DrawingGeometry> _process(const cv::Mat& image) override; };


// --- hatch3_way_pfm.h ---
class Hatch3WayPFM : public BaseMultiHatchPFM {
public: explicit Hatch3WayPFM(){} std::string name() const override{return "Hatch 3-Way";} protected: std::vector<float> getAngles() const override { return {0.0f, 45.0f, -45.0f}; } };


// --- hatch4_way_pfm.h ---
class Hatch4WayPFM : public BaseMultiHatchPFM {
public: explicit Hatch4WayPFM(){} std::string name() const override{return "Hatch 4-Way";} protected: std::vector<float> getAngles() const override { return {0.0f, 90.0f, 45.0f, -45.0f}; } };


// --- hatch5_way_pfm.h ---
class Hatch5WayPFM : public BaseMultiHatchPFM {
public: explicit Hatch5WayPFM(){} std::string name() const override{return "Hatch 5-Way";} protected: std::vector<float> getAngles() const override { return {0.0f, 90.0f, 45.0f, -45.0f, 22.5f}; } };


// --- hatch6_way_pfm.h ---
class Hatch6WayPFM : public BaseMultiHatchPFM {
public: explicit Hatch6WayPFM(){} std::string name() const override{return "Hatch 6-Way";} protected: std::vector<float> getAngles() const override { return {0.0f, 90.0f, 45.0f, -45.0f, 22.5f, -22.5f}; } };


// --- hatch_circular_scribbles_pfm.h ---
class HatchCircularScribblesPFM : public PathFindingModule {
public:
    explicit HatchCircularScribblesPFM();
    std::string name() const override { return "Hatch Circular Scribbles"; }
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- hatch_cross_pfm.h ---
class HatchCrossPFM : public BaseMultiHatchPFM {
public: explicit HatchCrossPFM(){} std::string name() const override{return "Hatch Cross";} protected: std::vector<float> getAngles() const override { return {45.0f, -45.0f}; } };


// --- hatch_diagonal1_pfm.h ---
class HatchDiagonal1PFM : public BaseMultiHatchPFM {
public: explicit HatchDiagonal1PFM(){} std::string name() const override{return "Hatch Diagonal 1";} protected: std::vector<float> getAngles() const override { return {45.0f}; } };


// --- hatch_diagonal2_pfm.h ---
class HatchDiagonal2PFM : public BaseMultiHatchPFM {
public: explicit HatchDiagonal2PFM(){} std::string name() const override{return "Hatch Diagonal 2";} protected: std::vector<float> getAngles() const override { return {-45.0f}; } };


// --- hatch_grid_pfm.h ---
class HatchGridPFM : public BaseMultiHatchPFM {
public: explicit HatchGridPFM(){} std::string name() const override{return "Hatch Grid";} protected: std::vector<float> getAngles() const override { return {0.0f, 90.0f}; } };


// --- hatch_horizontal_pfm.h ---
class HatchHorizontalPFM : public BaseMultiHatchPFM {
public: explicit HatchHorizontalPFM(){} std::string name() const override{return "Hatch Horizontal";} protected: std::vector<float> getAngles() const override { return {0.0f}; } };


// --- hatch_lines.h ---
class HatchLinesPFM : public PathFindingModule {
public:
    explicit HatchLinesPFM();
    std::string name()        const override { return "Hatch Lines"; }
    std::string description() const override { return "Cross-hatching based on image brightness."; }
    std::string category()    const override { return "Hatch"; }
protected:
    std::vector<PFMSetting>      defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- hatch_sawtooth_pfm.h ---
class HatchSawtoothPFM : public PathFindingModule {
public:
    explicit HatchSawtoothPFM();
    std::string name() const override { return "Hatch Sawtooth"; }
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- hatch_vertical_pfm.h ---
class HatchVerticalPFM : public BaseMultiHatchPFM {
public: explicit HatchVerticalPFM(){} std::string name() const override{return "Hatch Vertical";} protected: std::vector<float> getAngles() const override { return {90.0f}; } };


// --- labyrinth_classic_pfm.h ---
class LabyrinthClassicPFM : public PathFindingModule {
public:
    explicit LabyrinthClassicPFM();
    std::string name() const override { return "Labyrinth Classic"; }
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- labyrinth_roman_pfm.h ---
class LabyrinthRomanPFM : public LabyrinthClassicPFM {
public:
    explicit LabyrinthRomanPFM() {}
    std::string name() const override { return "Labyrinth Roman"; }
};


// --- layers_pfm.h ---
class LayersPFM : public PathFindingModule {
public:
    explicit LayersPFM();
    std::string name()     const override { return "Layers PFM"; }
    std::string category() const override { return "Composite"; }
    bool isPremium()   const override { return true; }
protected:
    std::vector<PFMSetting>      defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- lbg_circular_scribbles_pfm.h ---
class LBGCircularScribblesPFM : public AdaptiveCircularScribblesPFM {
public:
    explicit LBGCircularScribblesPFM() {}
    std::string name() const override { return "LBG Circular Scribbles"; }
    std::string category() const override { return "LBG"; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
};


// --- lbg_dashes_pfm.h ---
class LBGDashesPFM : public AdaptiveDashesPFM {
public:
    explicit LBGDashesPFM() {}
    std::string name() const override { return "LBG Dashes"; }
    std::string category() const override { return "LBG"; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
};


// --- lbg_diagram_pfm.h ---
class LBGDiagramPFM : public AdaptiveDiagramPFM {
public:
    explicit LBGDiagramPFM() {}
    std::string name() const override { return "LBG Diagram"; }
    std::string category() const override { return "LBG"; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
};


// --- lbg_letters_pfm.h ---
class LBGLettersPFM : public _LettersBasePFM {
public:
    explicit LBGLettersPFM();
    std::string name() const override { return "LBG Letters"; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    int getLloydIters() const override;
};


// --- lbg_shapes_pfm.h ---
class LBGShapesPFM : public AdaptiveShapesPFM {
public:
    explicit LBGShapesPFM() {}
    std::string name() const override { return "LBG Shapes"; }
    std::string category() const override { return "LBG"; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
};


// --- lbg_stippling_pfm.h ---
class LBGStipplingPFM : public AdaptiveStipplingPFM {
public:
    explicit LBGStipplingPFM() {}
    std::string name() const override { return "LBG Stippling"; }
    std::string category() const override { return "LBG"; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
};


// --- lbg_tree_pfm.h ---
class LBGTreePFM : public AdaptiveTreePFM {
public:
    explicit LBGTreePFM() {}
    std::string name() const override { return "LBG Tree"; }
    std::string category() const override { return "LBG"; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
};


// --- lbg_triangulation_pfm.h ---
class LBGTriangulationPFM : public AdaptiveTriangulationPFM {
public:
    explicit LBGTriangulationPFM() {}
    std::string name() const override { return "LBG Triangulation"; }
    std::string category() const override { return "LBG"; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
};


// --- lbgtsppfm.h ---
class LBGTSPPFM : public AdaptiveTSPPFM {
public:
    explicit LBGTSPPFM() {}
    std::string name() const override { return "LBG TSP"; }
    std::string category() const override { return "LBG"; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
};


// --- maze_circle_pfm.h ---
class MazeCirclePFM : public BaseMazePFM {
public: explicit MazeCirclePFM(){} std::string name() const override{return "Maze Circular";} protected: std::pair<float, float> transformPoint(float x, float y) override; };


// --- maze_hex_pfm.h ---
class MazeHexPFM : public BaseMazePFM {
public: explicit MazeHexPFM(){} std::string name() const override{return "Maze Hexagonal";} protected: std::pair<float, float> transformPoint(float x, float y) override; };


// --- maze_hilbert_pfm.h ---
class MazeHilbertPFM : public PathFindingModule {
public:
    explicit MazeHilbertPFM();
    std::string name() const override { return "Hilbert Curve"; }
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- maze_labyrinth_pfm.h ---
class MazeLabyrinthPFM : public BaseMazePFM {
public: explicit MazeLabyrinthPFM(){} std::string name() const override{return "Labyrinth";} };


// --- maze_peano_pfm.h ---
class MazePeanoPFM : public PathFindingModule {
public:
    explicit MazePeanoPFM();
    std::string name() const override { return "Peano Curve"; }
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- maze_rect_pfm.h ---
class MazeRectPFM : public BaseMazePFM {
public: explicit MazeRectPFM(){} std::string name() const override{return "Maze Rectangular";} };


// --- maze_tri_pfm.h ---
class MazeTriPFM : public BaseMazePFM {
public: explicit MazeTriPFM(){} std::string name() const override{return "Maze Triangular";} protected: std::pair<float, float> transformPoint(float x, float y) override; };


// --- maze_voronoi_pfm.h ---
class MazeVoronoiPFM : public BaseMazePFM {
public: explicit MazeVoronoiPFM(){} std::string name() const override{return "Maze Voronoi";} protected: std::pair<float, float> transformPoint(float x, float y) override; };


// --- mosaic_rectangles_pfm.h ---
class MosaicRectanglesPFM : public BaseMosaicPFM {
public: explicit MosaicRectanglesPFM(){} std::string name() const override{return "Mosaic Rectangles";} protected: std::vector<DrawingGeometry> _process(const cv::Mat& image) override; };


// --- mosaic_custom_pfm.h ---
class MosaicCustomPFM : public MosaicRectanglesPFM {
public: explicit MosaicCustomPFM(){} std::string name() const override{return "Mosaic Custom";} protected: std::vector<DrawingGeometry> _process(const cv::Mat& image) override; };


// --- mosaic_voronoi_pfm.h ---
class MosaicVoronoiPFM : public BaseMosaicPFM {
public: explicit MosaicVoronoiPFM(){} std::string name() const override{return "Mosaic Voronoi";} protected: std::vector<PFMSetting> defineSettings() const override; std::vector<DrawingGeometry> _process(const cv::Mat& image) override; };


// --- sketch_abstract_pfm.h ---
class SketchAbstractPFM : public PathFindingModule {
public:
    explicit SketchAbstractPFM();
    std::string name() const override { return "Sketch Abstract"; }
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- sketch_catmull_roms_pfm.h ---
class SketchCatmullRomsPFM : public PathFindingModule {
public:
    explicit SketchCatmullRomsPFM();
    std::string name() const override { return "Sketch Catmull-Roms"; }
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- sketch_cubic_beziers2_pfm.h ---
class SketchCubicBeziers2PFM : public PathFindingModule {
public:
    explicit SketchCubicBeziers2PFM();
    std::string name() const override { return "Sketch Cubic Beziers 2"; }
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- sketch_cubic_beziers_pfm.h ---
class SketchCubicBeziersPFM : public PathFindingModule {
public:
    explicit SketchCubicBeziersPFM();
    std::string name()        const override { return "Sketch Cubic Beziers"; }
    std::string description() const override { return "Transforms an image into Cubic Bezier curves."; }
    std::string category()    const override { return "Sketch"; }
    bool    isPremium()   const override { return true; }
protected:
    std::vector<PFMSetting>      defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- sketch_curves.h ---
enum class CurveOutputMode { CatmullRom, QuadBezier, CubicBezier };

std::vector<PFMSetting> makeSketchCommonSettings();
std::vector<DrawingGeometry> runSketchLoop(
    PathFindingModule* pfm,
    const cv::Mat& image,
    CurveOutputMode mode,
    int curveSmooth = 10,
    double curveAlpha = 0.5);


// --- sketch_curves_pfm.h ---
class SketchCurvesPFM : public PathFindingModule {
public:
    explicit SketchCurvesPFM();
    std::string name()        const override { return "Sketch Curves"; }
    std::string description() const override { return "Transforms an image into smooth curves using brightness data."; }
    std::string category()    const override { return "Sketch"; }
    bool    isPremium()   const override { return true; }
protected:
    std::vector<PFMSetting>      defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- sketch_delaunay_pfm.h ---
class SketchDelaunayPFM : public PathFindingModule {
public:
    explicit SketchDelaunayPFM();
    std::string name() const override { return "Sketch Delaunay"; }
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- sketch_flow_fields_pfm.h ---
class SketchFlowFieldsPFM : public PathFindingModule {
public:
    explicit SketchFlowFieldsPFM();
    std::string name() const override { return "Sketch Flow Fields"; }
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- sketch_lines.h ---
class SketchLinesPFM : public PathFindingModule {
public:
    explicit SketchLinesPFM();
    std::string name()        const override { return "Sketch Lines"; }
    std::string description() const override { return "Transforms an image into lines using brightness data."; }
    std::string category()    const override { return "Sketch"; }
protected:
    std::vector<PFMSetting>      defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;

private:
    void eraseLine(cv::Mat& img, double x0, double y0, double x1, double y1,
                   double eraseMin, double eraseMax,
                   double radiusMin, double radiusMax,
                   double tone, int w, int h);
};


// --- sketch_quad_beziers2_pfm.h ---
class SketchQuadBeziers2PFM : public SketchCubicBeziers2PFM {
public:
    explicit SketchQuadBeziers2PFM() {}
    std::string name() const override { return "Sketch Quad Beziers 2"; }
protected:
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- sketch_quad_beziers_pfm.h ---
class SketchQuadBeziersPFM : public PathFindingModule {
public:
    explicit SketchQuadBeziersPFM();
    std::string name()        const override { return "Sketch Quad Beziers"; }
    std::string description() const override { return "Transforms an image into Quadratic Bezier curves."; }
    std::string category()    const override { return "Sketch"; }
    bool    isPremium()   const override { return true; }
protected:
    std::vector<PFMSetting>      defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- sketch_radial_pfm.h ---
class SketchRadialPFM : public PathFindingModule {
public:
    explicit SketchRadialPFM();
    std::string name() const override { return "Sketch Radial"; }
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- sketch_scribble_pfm.h ---
class SketchScribblePFM : public PathFindingModule {
public:
    explicit SketchScribblePFM();
    std::string name() const override { return "Sketch Scribble"; }
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- sketch_shapes.h ---
class SketchShapesPFM : public PathFindingModule {
public:
    explicit SketchShapesPFM();
    std::string name()        const override { return "Sketch Shapes"; }
    std::string description() const override { return "Transforms an image into assorted shapes using brightness data."; }
    std::string category()    const override { return "Sketch"; }
protected:
    std::vector<PFMSetting>      defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- sketch_sobel_edges_pfm.h ---
class SketchSobelEdgesPFM : public PathFindingModule {
public:
    explicit SketchSobelEdgesPFM();
    std::string name() const override { return "Sketch Sobel Edges"; }
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- sketch_spirals.h ---
class SketchSpiralsPFM : public PathFindingModule {
public:
    explicit SketchSpiralsPFM();
    std::string name()        const override { return "Sketch Spirals"; }
    std::string description() const override { return "Draws a continuous spiral, tightening in dark areas."; }
    std::string category()    const override { return "Sketch"; }
protected:
    std::vector<PFMSetting>      defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- sketch_squares.h ---
class SketchSquaresPFM : public PathFindingModule {
public:
    explicit SketchSquaresPFM();
    std::string name()        const override { return "Sketch Squares"; }
    std::string description() const override { return "Transforms an image into squares using brightness data."; }
    std::string category()    const override { return "Sketch"; }
protected:
    std::vector<PFMSetting>      defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- sketch_superformula_pfm.h ---
class SketchSuperformulaPFM : public PathFindingModule {
public:
    explicit SketchSuperformulaPFM();
    std::string name() const override { return "Sketch Superformula"; }
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- sketch_sweeping_curves_pfm.h ---
class SketchSweepingCurvesPFM : public SketchCubicBeziers2PFM {
public:
    explicit SketchSweepingCurvesPFM();
    std::string name() const override;
};


// --- sketch_voronoi_pfm.h ---
class SketchVoronoiPFM : public PathFindingModule {
public:
    explicit SketchVoronoiPFM();
    std::string name() const override { return "Sketch Voronoi"; }
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- sketch_waves.h ---
class SketchWavesPFM : public PathFindingModule {
public:
    explicit SketchWavesPFM();
    std::string name()        const override { return "Sketch Waves"; }
    std::string description() const override { return "Draws sine waves varying amplitude by brightness."; }
    std::string category()    const override { return "Sketch"; }
protected:
    std::vector<PFMSetting>      defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- spiral_circular_scribbles_pfm.h ---
class SpiralCircularScribblesPFM : public PathFindingModule {
public:
    explicit SpiralCircularScribblesPFM();
    std::string name() const override { return "Spiral Circular Scribbles"; }
    std::string category() const override { return "Spiral"; }
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- spiral_sawtooth_pfm.h ---
class SpiralSawtoothPFM : public PathFindingModule {
public:
    explicit SpiralSawtoothPFM();
    std::string name() const override { return "Spiral Sawtooth"; }
    std::string category() const override { return "Spiral"; }
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- stipple_chaos_pfm.h ---
class StippleChaosPFM : public BaseStipplePFM {
public: explicit StippleChaosPFM(){} std::string name() const override{return "Stipple Chaos";} protected: Path generateShape(float cx, float cy, float r) override; };


// --- stipple_dots.h ---
class StippleDotsPFM : public PathFindingModule {
public:
    explicit StippleDotsPFM();
    std::string name()        const override { return "Stipple Dots"; }
    std::string description() const override { return "Pointillism based on image brightness."; }
    std::string category()    const override { return "Stipple"; }
protected:
    std::vector<PFMSetting>      defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- stipple_circles_pfm.h ---
class StippleCirclesPFM : public BaseStipplePFM {
public: explicit StippleCirclesPFM(){} std::string name() const override{return "Stipple Circles";} protected: Path generateShape(float cx, float cy, float r) override; };


// --- stipple_crosses_pfm.h ---
class StippleCrossesPFM : public BaseStipplePFM {
public: explicit StippleCrossesPFM(){} std::string name() const override{return "Stipple Crosses";} protected: Path generateShape(float cx, float cy, float r) override; };


// --- stipple_hexagons_pfm.h ---
class StippleHexagonsPFM : public BaseStipplePFM {
public: explicit StippleHexagonsPFM(){} std::string name() const override{return "Stipple Hexagons";} protected: Path generateShape(float cx, float cy, float r) override; };


// --- stipple_layers_pfm.h ---
class StippleLayersPFM : public PathFindingModule {
public:
    explicit StippleLayersPFM();
    std::string name()     const override { return "Stipple Layers"; }
    std::string category() const override { return "Composite"; }
    bool isPremium()   const override { return true; }
protected:
    std::vector<PFMSetting>      defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- stipple_lines_pfm.h ---
class StippleLinesPFM : public BaseStipplePFM {
public: explicit StippleLinesPFM(){} std::string name() const override{return "Stipple Lines";} protected: Path generateShape(float cx, float cy, float r) override; };


// --- stipple_squares_pfm.h ---
class StippleSquaresPFM : public BaseStipplePFM {
public: explicit StippleSquaresPFM(){} std::string name() const override{return "Stipple Squares";} protected: Path generateShape(float cx, float cy, float r) override; };


// --- stipple_stars_pfm.h ---
class StippleStarsPFM : public BaseStipplePFM {
public: explicit StippleStarsPFM(){} std::string name() const override{return "Stipple Stars";} protected: Path generateShape(float cx, float cy, float r) override; };


// --- stipple_triangles_pfm.h ---
class StippleTrianglesPFM : public BaseStipplePFM {
public: explicit StippleTrianglesPFM(){} std::string name() const override{return "Stipple Triangles";} protected: Path generateShape(float cx, float cy, float r) override; };


// --- stipple_variable_circles_pfm.h ---
class StippleVariableCirclesPFM : public BaseStipplePFM {
public: explicit StippleVariableCirclesPFM(){} std::string name() const override{return "Stipple Variable Circles";} protected: Path generateShape(float cx, float cy, float r) override; };


// --- stipple_variable_squares_pfm.h ---
class StippleVariableSquaresPFM : public BaseStipplePFM {
public: explicit StippleVariableSquaresPFM(){} std::string name() const override{return "Stipple Variable Squares";} protected: Path generateShape(float cx, float cy, float r) override; };


// --- streamlines_edge_field_pfm.h ---
class StreamlinesEdgeFieldPFM : public BaseStreamlinePFM {
public:
    explicit StreamlinesEdgeFieldPFM() {}
    std::string name() const override { return "Streamlines Edge Field"; }
protected:
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- streamlines_flow_field_pfm.h ---
class StreamlinesFlowFieldPFM : public BaseStreamlinePFM {
public:
    explicit StreamlinesFlowFieldPFM() {}
    std::string name() const override { return "Streamlines Flow Field"; }
protected:
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- streamlines_superformula_pfm.h ---
class StreamlinesSuperformulaPFM : public BaseStreamlinePFM {
public:
    explicit StreamlinesSuperformulaPFM() {}
    std::string name() const override { return "Streamlines Superformula"; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- tsp_outline_pfm.h ---
class TSPOutlinePFM : public BaseTSPPFM {
public:
    explicit TSPOutlinePFM() {}
    std::string name() const override { return "TSP Outline"; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<double> getProbabilities(const cv::Mat& image) override;
};


// --- tsp_shading_pfm.h ---
class TSPShadingPFM : public BaseTSPPFM {
public:
    explicit TSPShadingPFM() {}
    std::string name() const override { return "TSP Shading"; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<double> getProbabilities(const cv::Mat& image) override;
};


// --- tsp_stipple_pfm.h ---
class TSPStipplePFM : public BaseTSPPFM {
public:
    explicit TSPStipplePFM() {}
    std::string name() const override { return "TSP Stipple"; }
protected:
    std::vector<double> getProbabilities(const cv::Mat& image) override;
};


// --- tsp_voronoi_pfm.h ---
class TSPVoronoiPFM : public BaseTSPPFM {
public:
    explicit TSPVoronoiPFM();
    std::string name() const override { return "TSP Voronoi"; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<double> getProbabilities(const cv::Mat& image) override;
};


// --- tspmstpfm.h ---
class TSPMSTPFM : public PathFindingModule {
public:
    explicit TSPMSTPFM();
    std::string name() const override { return "Minimum Spanning Tree"; }
    bool isPremium() const override { return true; }
protected:
    std::vector<PFMSetting> defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// --- voronoi_circles_pfm.h ---
class VoronoiCirclesPFM : public BaseVoronoiExtraPFM {
public: explicit VoronoiCirclesPFM(){} std::string name() const override{return "Voronoi Circles";} protected: std::vector<DrawingGeometry> _process(const cv::Mat& image) override; };


// --- voronoi_dashes_pfm.h ---
class VoronoiDashesPFM : public BaseVoronoiExtraPFM {
public: explicit VoronoiDashesPFM(){} std::string name() const override{return "Voronoi Dashes";} protected: std::vector<DrawingGeometry> _process(const cv::Mat& image) override; };


// --- voronoi_diagram_pfm.h ---
class VoronoiDiagramPFM : public BaseVoronoiExtraPFM {
public: explicit VoronoiDiagramPFM(){} std::string name() const override{return "Voronoi Diagram";} protected: std::vector<DrawingGeometry> _process(const cv::Mat& image) override; };


// --- voronoi_letters_pfm.h ---
class VoronoiLettersPFM : public _LettersBasePFM {
public:
    explicit VoronoiLettersPFM() {}
    std::string name() const override { return "Voronoi Letters"; }
protected:
    int getLloydIters() const override;
};


// --- voronoi_shapes_pfm.h ---
class VoronoiShapesPFM : public BaseVoronoiExtraPFM {
public: explicit VoronoiShapesPFM(){} std::string name() const override{return "Voronoi Shapes";} protected: std::vector<DrawingGeometry> _process(const cv::Mat& image) override; };


// --- voronoi_stippling_pfm.h ---
class VoronoiStipplingPFM : public BaseVoronoiExtraPFM {
public: explicit VoronoiStipplingPFM(){} std::string name() const override{return "Voronoi Stippling";} protected: std::vector<DrawingGeometry> _process(const cv::Mat& image) override; };


// --- voronoi_tree_pfm.h ---
class VoronoiTreePFM : public BaseVoronoiExtraPFM {
public: explicit VoronoiTreePFM(){} std::string name() const override{return "Voronoi Tree";} protected: std::vector<DrawingGeometry> _process(const cv::Mat& image) override; };


// --- voronoi_triangulation_pfm.h ---
class VoronoiTriangulationPFM : public BaseVoronoiExtraPFM {
public: explicit VoronoiTriangulationPFM(){} std::string name() const override{return "Voronoi Triangulation";} protected: std::vector<DrawingGeometry> _process(const cv::Mat& image) override; };


// --- voronoi_tsppfm.h ---
class VoronoiTSPPFM : public BaseVoronoiExtraPFM {
public: explicit VoronoiTSPPFM(){} std::string name() const override{return "Voronoi TSP";} protected: std::vector<DrawingGeometry> _process(const cv::Mat& image) override; };


// --- _letters_base_pfm.cpp ---
inline std::map<char, std::vector<std::pair<float, float>>> LETTER_GLYPHS = {
    {'A', {{0.1, 1}, {0.5, 0}, {0.9, 1}, {0.7, 0.55}, {0.3, 0.55}}},
    {'B', {{0.2, 0}, {0.2, 1}, {0.6, 1}, {0.8, 0.85}, {0.6, 0.5}, {0.8, 0.15}, {0.6, 0}, {0.2, 0}}},
    {'C', {{0.9, 0.15}, {0.6, 0}, {0.3, 0}, {0.1, 0.3}, {0.1, 0.7}, {0.3, 1}, {0.6, 1}, {0.9, 0.85}}},
    {'D', {{0.2, 0}, {0.2, 1}, {0.55, 1}, {0.85, 0.75}, {0.85, 0.25}, {0.55, 0}, {0.2, 0}}},
    {'E', {{0.8, 0}, {0.2, 0}, {0.2, 1}, {0.8, 1}, {0.2, 0.5}, {0.65, 0.5}}},
    {'F', {{0.2, 0}, {0.2, 1}, {0.8, 1}, {0.2, 0.5}, {0.7, 0.5}}},
    {'G', {{0.9, 0.2}, {0.6, 0}, {0.3, 0.05}, {0.1, 0.4}, {0.1, 0.7}, {0.35, 1}, {0.75, 0.9}, {0.75, 0.55}, {0.5, 0.55}}},
    {'H', {{0.2, 0}, {0.2, 1}, {0.2, 0.5}, {0.8, 0.5}, {0.8, 1}, {0.8, 0}}},
    {'I', {{0.35, 0}, {0.65, 0}, {0.5, 0}, {0.5, 1}, {0.35, 1}, {0.65, 1}}},
    {'J', {{0.7, 0}, {0.3, 0}, {0.3, 0.8}, {0.5, 1}, {0.75, 0.85}}},
    {'K', {{0.2, 0}, {0.2, 1}, {0.2, 0.5}, {0.85, 1}, {0.25, 0.5}, {0.85, 0}}},
    {'L', {{0.2, 1}, {0.2, 0}, {0.85, 0}}},
    {'M', {{0.1, 0}, {0.1, 1}, {0.5, 0.5}, {0.9, 1}, {0.9, 0}}},
    {'N', {{0.15, 0}, {0.15, 1}, {0.85, 0}, {0.85, 1}}},
    {'O', {{0.5, 0}, {0.15, 0.15}, {0, 0.5}, {0.15, 0.85}, {0.5, 1}, {0.85, 0.85}, {1, 0.5}, {0.85, 0.15}, {0.5, 0}}},
    {'P', {{0.2, 0}, {0.2, 1}, {0.65, 1}, {0.85, 0.8}, {0.65, 0.5}, {0.2, 0.5}}},
    {'Q', {{0.5, 0}, {0.15, 0.15}, {0, 0.5}, {0.2, 0.85}, {0.5, 1}, {0.85, 0.85}, {1, 0.5}, {0.85, 0.15}, {0.5, 0}, {0.75, 0.25}, {1, 0}}},
    {'R', {{0.2, 0}, {0.2, 1}, {0.65, 1}, {0.85, 0.8}, {0.65, 0.5}, {0.2, 0.5}, {0.85, 0}}},
    {'S', {{0.85, 0.85}, {0.55, 1}, {0.25, 0.9}, {0.15, 0.65}, {0.45, 0.5}, {0.75, 0.35}, {0.85, 0.15}, {0.55, 0}, {0.25, 0.1}}},
    {'T', {{0, 1}, {1, 1}, {0.5, 1}, {0.5, 0}}},
    {'U', {{0.15, 1}, {0.15, 0.25}, {0.35, 0}, {0.65, 0}, {0.85, 0.25}, {0.85, 1}}},
    {'V', {{0, 1}, {0.5, 0}, {1, 1}}},
    {'W', {{0, 1}, {0.25, 0}, {0.5, 0.6}, {0.75, 0}, {1, 1}}},
    {'X', {{0, 0}, {1, 1}, {0, 1}, {1, 0}}},
    {'Y', {{0, 1}, {0.5, 0.45}, {1, 1}, {0.5, 0.45}, {0.5, 0}}},
    {'Z', {{0.1, 1}, {0.9, 1}, {0.1, 0}, {0.9, 0}}}
};

inline Path letter_glyph_path(char letter, float cx, float cy, float size) {
    if (LETTER_GLYPHS.find(letter) == LETTER_GLYPHS.end()) letter = 'O';
    Path p;
    for (const auto& pt : LETTER_GLYPHS[letter]) {
        p.push_back({cx + (pt.first - 0.5f) * size, cy + (0.5f - pt.second) * size});
    }
    return p;
}

static char letter_for_brightness(float darkness) {
    int idx = int(std::clamp(darkness / 255.0f, 0.0f, 0.999f) * 26.0f);
    const char* letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    return letters[idx];
}

// nearest_seed_radius: already defined above

// -------------------------------------------------------------------------
_LettersBasePFM::_LettersBasePFM() {
    initSettings();
}

std::vector<PFMSetting> _LettersBasePFM::defineSettings() const {
    return {
        {"cell_count", "Cell Count", SettingType::Integer, 800, SettingValue(), 50, 20000, 50, 20000, 100},
        {"lloyd_iterations", "Lloyd Iterations", SettingType::Integer, 0, SettingValue(), 0, 20, 0, 20, 1},
        {"min_brightness", "Min Brightness", SettingType::Number, 0.0, SettingValue(), 0.0, 255.0, 0.0, 255.0, 1.0},
        {"letter_scale", "Letter Scale", SettingType::Number, 0.85, SettingValue(), 0.2, 1.5, 0.2, 1.5, 0.05}
    };
}

std::vector<DrawingGeometry> _LettersBasePFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int cell_count = m_settings["cell_count"].toInt();
    int lloyd_iters = getLloydIters();
    float min_brightness = m_settings["min_brightness"].toDouble();
    float letter_scale = m_settings["letter_scale"].toDouble();

    std::vector<double> probs(w * h, 0.0);
    std::vector<double> original_dark(w * h, 0.0);
    double sum = 0.0;
    
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            original_dark[y * w + x] = d;
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    std::vector<cv::Point2f> pts;
    for (int i = 0; i < cell_count; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        pts.push_back(cv::Point2f(idx % w, idx / w));
    }

    if (lloyd_iters > 0) {
        int step = std::max(1, int(std::sqrt(h * w / 20000)));
        std::vector<cv::Point2f> sub_coords;
        std::vector<double> sub_w;
        for (int y = 0; y < h; y += step) {
            for (int x = 0; x < w; x += step) {
                sub_coords.push_back(cv::Point2f(x, y));
                sub_w.push_back(probs[y * w + x]);
            }
        }
        
        for (int iter = 0; iter < lloyd_iters; ++iter) {
            if (isCancelled()) return {};
            emitProgress(float(iter) / lloyd_iters, 0, "Lloyd Relaxation...");
            
            std::vector<cv::Point2f> new_pts(pts.size(), cv::Point2f(0,0));
            std::vector<double> weight_sums(pts.size(), 0.0);

            for (size_t i = 0; i < sub_coords.size(); ++i) {
                float best_d = 1e12f;
                int best_c = -1;
                for (size_t c = 0; c < pts.size(); ++c) {
                    float dx = sub_coords[i].x - pts[c].x;
                    float dy = sub_coords[i].y - pts[c].y;
                    float d = dx*dx + dy*dy;
                    if (d < best_d) { best_d = d; best_c = c; }
                }
                if (best_c != -1) {
                    new_pts[best_c].x += sub_coords[i].x * sub_w[i];
                    new_pts[best_c].y += sub_coords[i].y * sub_w[i];
                    weight_sums[best_c] += sub_w[i];
                }
            }

            for (size_t c = 0; c < pts.size(); ++c) {
                if (weight_sums[c] > 1e-9) {
                    pts[c].x = new_pts[c].x / weight_sums[c];
                    pts[c].y = new_pts[c].y / weight_sums[c];
                }
            }
        }
    }

    std::vector<cv::Point2f> filtered_pts;
    for (const auto& p : pts) {
        int xi = std::clamp(int(p.x), 0, w - 1);
        int yi = std::clamp(int(p.y), 0, h - 1);
        if (original_dark[yi * w + xi] >= min_brightness) {
            filtered_pts.push_back(p);
        }
    }

    std::vector<DrawingGeometry> geoms;
    for (size_t i = 0; i < filtered_pts.size(); ++i) {
        if (isCancelled()) break;
        if (i % 40 == 0) emitProgress(float(i) / std::max<size_t>(1, filtered_pts.size()), geoms.size(), "Placing letters...");
        
        float cx = filtered_pts[i].x;
        float cy = filtered_pts[i].y;
        
        int xi = std::clamp(int(cx), 0, w - 1);
        int yi = std::clamp(int(cy), 0, h - 1);
        float dark = original_dark[yi * w + xi];
        
        char letter = letter_for_brightness(dark);
        float r = nearest_seed_radius(cx, cy, filtered_pts) * 2.0f * letter_scale;
        
        Path path = letter_glyph_path(letter, cx, cy, std::max(4.0f, r));
        if (path.size() >= 2) {
            DrawingGeometry dg;
            dg.path = path;
            geoms.push_back(dg);
        }
    }

    return geoms;
}


// --- base_adaptive_pfm.cpp ---
// ---------------------------------------------------------------------------

BaseAdaptivePFM::BaseAdaptivePFM() {
    initSettings();
}

std::vector<PFMSetting> BaseAdaptivePFM::defineSettings() const {
    return {
        {"plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0, SettingValue(), 0.1, 2.0,   0.1, 2.0,   0.05},
        {"random_seed",         "Random Seed",         SettingType::Integer, 42,  SettingValue(), 0,   999999, 0,   999999, 1},
        {"cell_count",          "Cell Count",          SettingType::Integer, 800, SettingValue(), 50,  20000,  50,  20000,  50},
        {"lloyd_iterations",    "Lloyd Iterations",    SettingType::Integer, 3,   SettingValue(), 0,   20,     0,   10,     1},
        {"min_brightness",      "Min Brightness (%)",  SettingType::Number,  80.0, SettingValue(), 0.0, 100.0, 0.0, 100.0, 1.0},
    };
}

std::vector<cv::Point2f> BaseAdaptivePFM::getSeeds(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int cellCount   = m_settings["cell_count"].toInt();
    int lloydIters  = get("lloyd_iterations").toInt();
    float minBright = (float)(m_settings["min_brightness"].toDouble() / 100.0) * 255.0f;

    // Build darkness weight map
    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    // Initial weighted sampling
    std::vector<cv::Point2f> pts;
    pts.reserve(cellCount);
    for (int i = 0; i < cellCount; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        pts.push_back(cv::Point2f(idx % w, idx / w));
    }

    // Lloyd relaxation (simplified — move toward dark neighbourhood)
    for (int iter = 0; iter < lloydIters && !isCancelled(); ++iter) {
        emitProgress(float(iter) / lloydIters, 0, "Lloyd relaxation...");
        for (auto& pt : pts) {
            int px = (int)pt.x, py = (int)pt.y;
            double bestW = -1.0;
            float bx = pt.x, by = pt.y;
            for (int dy = -5; dy <= 5; ++dy)
                for (int dx = -5; dx <= 5; ++dx) {
                    int nx = px + dx, ny = py + dy;
                    if (nx < 0 || nx >= w || ny < 0 || ny >= h) continue;
                    double ww = probs[ny * w + nx];
                    if (ww > bestW) { bestW = ww; bx = nx; by = ny; }
                }
            pt = {bx, by};
        }
    }

    // Filter by min brightness
    std::vector<cv::Point2f> filtered;
    for (const auto& p : pts) {
        int xi = std::clamp((int)p.x, 0, w - 1);
        int yi = std::clamp((int)p.y, 0, h - 1);
        float bright = image.at<uchar>(yi, xi);
        if (bright <= (255.0f - minBright))  // only dark enough pixels
            filtered.push_back(p);
    }
    return filtered;
}


// --- base_grid_pfm.cpp ---
BaseGridPFM::BaseGridPFM() {
    initSettings();
}

std::vector<PFMSetting> BaseGridPFM::defineSettings() const {
    return {
        {"cols", "Columns", SettingType::Integer, 40, SettingValue(), 2, 200, 2, 200, 1},
        {"rows", "Rows", SettingType::Integer, 40, SettingValue(), 2, 200, 2, 200, 1},
        {"threshold", "Darkness Threshold", SettingType::Number, 50.0, SettingValue(), 0.0, 255.0, 0.0, 255.0, 1.0}
    };
}


// --- base_maze_pfm.cpp ---
inline Path generate_dfs_maze(int width, int height, BaseMazePFM* pfm) {
    cv::Mat maze = cv::Mat::zeros(height, width, CV_8UC1);
    std::vector<std::pair<int, int>> stack;
    stack.push_back({0, 0});
    maze.at<uint8_t>(0, 0) = 1;
    Path path;
    int dx[] = {0, 2, 0, -2};
    int dy[] = {2, 0, -2, 0};
    while (!stack.empty()) {
        auto [x, y] = stack.back();
        path.push_back({x * 10.0, y * 10.0});
        std::vector<std::pair<int, int>> neighbors;
        for (int i = 0; i < 4; ++i) {
            int nx = x + dx[i], ny = y + dy[i];
            if (nx >= 0 && nx < width && ny >= 0 && ny < height && maze.at<uint8_t>(ny, nx) == 0) {
                neighbors.push_back({nx, ny});
            }
        }
        if (!neighbors.empty()) {
            auto [nx, ny] = neighbors[int(pfm->randUniform(0, neighbors.size() - 0.001))];
            maze.at<uint8_t>(ny, nx) = 1;
            maze.at<uint8_t>(y + (ny - y) / 2, x + (nx - x) / 2) = 1;
            stack.push_back({nx, ny});
        } else {
            stack.pop_back();
        }
    }
    return path;
}

BaseMazePFM::BaseMazePFM() {
    initSettings();
}

std::vector<PFMSetting> BaseMazePFM::defineSettings() const {
    return {
        {"complexity", "Complexity", SettingType::Integer, 10, SettingValue(), 1, 50, 1, 50, 1}
    };
}

std::pair<float, float> BaseMazePFM::transformPoint(float x, float y) { return {x, y}; }

std::vector<DrawingGeometry> BaseMazePFM::_process(const cv::Mat& image) {
    int comps = m_settings["complexity"].toInt();
    int gw = std::max(5, image.cols / (comps * 2));
    int gh = std::max(5, image.rows / (comps * 2));
    Path path = generate_dfs_maze(gw, gh, this);
    if (path.empty()) return {};

    for (auto& p : path) {
        auto t = transformPoint(p.first, p.second);
        p.first = t.first; p.second = t.second;
    }

    float max_x = 1e-5f, max_y = 1e-5f;
    for (const auto& p : path) {
        if (p.first > max_x) max_x = p.first;
        if (p.second > max_y) max_y = p.second;
    }

    Path scaled;
    for (const auto& p : path) {
        scaled.push_back({(p.first / max_x) * image.cols, (p.second / max_y) * image.rows});
    }

    DrawingGeometry dg; dg.path = scaled;
    return {dg};
}


// --- base_mosaic_pfm.cpp ---
BaseMosaicPFM::BaseMosaicPFM() {
    initSettings();
}

std::vector<PFMSetting> BaseMosaicPFM::defineSettings() const {
    return {
        {"cols", "Columns", SettingType::Integer, 20, SettingValue(), 2, 100, 2, 100, 1},
        {"rows", "Rows", SettingType::Integer, 20, SettingValue(), 2, 100, 2, 100, 1},
        {"threshold", "Darkness Threshold", SettingType::Number, 40.0, SettingValue(), 0.0, 255.0, 0.0, 255.0, 1.0}
    };
}


// --- base_multi_hatch_pfm.cpp ---
BaseMultiHatchPFM::BaseMultiHatchPFM() {
    initSettings();
}

std::vector<PFMSetting> BaseMultiHatchPFM::defineSettings() const {
    return {
        {"spacing", "Spacing", SettingType::Number, 5.0, SettingValue(), 1.0, 50.0, 1.0, 50.0, 0.5},
        {"threshold", "Darkness Threshold", SettingType::Percentage, 50.0, SettingValue(), 0.0, 100.0, 0.0, 100.0, 1.0}
    };
}

std::vector<DrawingGeometry> BaseMultiHatchPFM::_process(const cv::Mat& image) {
    float spacing = m_settings["spacing"].toDouble();
    float thresh = (m_settings["threshold"].toDouble() / 100.0f) * 255.0f;
    std::vector<float> angles = getAngles();
    
    std::vector<DrawingGeometry> geoms;
    int w = image.cols;
    int h = image.rows;
    float diag_len = std::hypot(w, h);
    int num_lines = int(diag_len / std::max(1.0f, spacing));
    
    for (float angle : angles) {
        if (isCancelled()) break;
        float rad = angle * 3.141592653589793 / 180.0f;
        float dx = std::cos(rad);
        float dy = std::sin(rad);
        
        for (int i = 0; i < num_lines; ++i) {
            if (isCancelled()) break;
            if (i % 20 == 0) emitProgress(float(i) / num_lines, geoms.size(), "" /*progress msg*/);
            
            float offset = (i - num_lines / 2.0f) * spacing;
            float cx = w / 2.0f + offset * -dy;
            float cy = h / 2.0f + offset * dx;
            float x1 = cx - dx * diag_len;
            float y1 = cy - dy * diag_len;
            float x2 = cx + dx * diag_len;
            float y2 = cy + dy * diag_len;
            
            Path path;
            int steps = std::max(1, int(diag_len));
            for (int step = 0; step < steps; ++step) {
                float px = x1 + (x2 - x1) * (float(step) / steps);
                float py = y1 + (y2 - y1) * (float(step) / steps);
                if (px >= 0 && px < w && py >= 0 && py < h) {
                    if ((255.0f - image.at<float>(int(py), int(px))) > thresh) {
                        path.push_back({px, py});
                    } else {
                        if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
                        path.clear();
                    }
                }
            }
            if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
        }
    }
    return geoms;
}


// --- base_stipple_pfm.cpp ---
BaseStipplePFM::BaseStipplePFM() {
    initSettings();
}

std::vector<PFMSetting> BaseStipplePFM::defineSettings() const {
    return {
        {"num_shapes", "Number of Shapes", SettingType::Integer, 5000, SettingValue(), 100, 50000, 100, 50000, 100},
        {"shape_size", "Shape Size", SettingType::Number, 1.0, SettingValue(), 0.1, 10.0, 0.1, 10.0, 0.1}
    };
}

std::vector<DrawingGeometry> BaseStipplePFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int num_shapes = m_settings["num_shapes"].toInt();
    double size = m_settings["shape_size"].toDouble();

    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    std::vector<DrawingGeometry> geoms;
    for (int i = 0; i < num_shapes; ++i) {
        if (isCancelled()) break;
        if (i % 500 == 0) emitProgress(float(i) / num_shapes, geoms.size(), "Stippling...");

        int idx = weightedChoice(probs);
        int cx = idx % w;
        int cy = idx / w;
        double local = probs[idx] * sum / 255.0; // recover brightness
        double r = size * (0.3 + 0.7 * local);

        DrawingGeometry dg;
        dg.path = generateShape(cx, cy, r);
        geoms.push_back(dg);
    }
    return geoms;
}


// --- base_streamline_pfm.cpp ---
BaseStreamlinePFM::BaseStreamlinePFM() {
    initSettings();
}

std::vector<PFMSetting> BaseStreamlinePFM::defineSettings() const {
    return {
        {"line_count", "Line Count", SettingType::Integer, 120, SettingValue(), 10, 2000, 10, 2000, 50},
        {"max_length", "Max Length", SettingType::Integer, 200, SettingValue(), 20, 2000, 20, 2000, 10},
        {"step_size", "Step Size", SettingType::Number, 1.5, SettingValue(), 0.5, 5.0, 0.5, 5.0, 0.1}
    };
}


// --- base_voronoi_extra_pfm.cpp ---
BaseVoronoiExtraPFM::BaseVoronoiExtraPFM() {
    initSettings();
}

std::vector<PFMSetting> BaseVoronoiExtraPFM::defineSettings() const {
    return {
        {"cell_count", "Cell Count", SettingType::Integer, 800, SettingValue(), 50, 20000, 50, 20000, 100},
        {"lloyd_iterations", "Lloyd Iterations", SettingType::Integer, 5, SettingValue(), 0, 20, 0, 20, 1},
        {"min_brightness", "Min Brightness", SettingType::Number, 0.0, SettingValue(), 0.0, 255.0, 0.0, 255.0, 1.0}
    };
}

std::vector<cv::Point2f> BaseVoronoiExtraPFM::getSeeds(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int cell_count = m_settings["cell_count"].toInt();
    int lloyd_iters = get("lloyd_iterations").toInt();
    float min_brightness = m_settings["min_brightness"].toDouble();

    std::vector<double> probs(w * h, 0.0);
    std::vector<double> original_dark(w * h, 0.0);
    double sum = 0.0;
    
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            original_dark[y * w + x] = d;
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    std::vector<cv::Point2f> pts;
    for (int i = 0; i < cell_count; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        pts.push_back(cv::Point2f(idx % w, idx / w));
    }

    if (lloyd_iters > 0) {
        int step = std::max(1, int(std::sqrt(h * w / 20000)));
        std::vector<cv::Point2f> sub_coords;
        std::vector<double> sub_w;
        for (int y = 0; y < h; y += step) {
            for (int x = 0; x < w; x += step) {
                sub_coords.push_back(cv::Point2f(x, y));
                sub_w.push_back(probs[y * w + x]);
            }
        }
        
        for (int iter = 0; iter < lloyd_iters; ++iter) {
            if (isCancelled()) return {};
            emitProgress(float(iter) / lloyd_iters, 0, "Lloyd Relaxation...");
            
            std::vector<cv::Point2f> new_pts(pts.size(), cv::Point2f(0,0));
            std::vector<double> weight_sums(pts.size(), 0.0);

            for (size_t i = 0; i < sub_coords.size(); ++i) {
                float best_d = 1e12f;
                int best_c = -1;
                for (size_t c = 0; c < pts.size(); ++c) {
                    float dx = sub_coords[i].x - pts[c].x;
                    float dy = sub_coords[i].y - pts[c].y;
                    float d = dx*dx + dy*dy;
                    if (d < best_d) { best_d = d; best_c = c; }
                }
                if (best_c != -1) {
                    new_pts[best_c].x += sub_coords[i].x * sub_w[i];
                    new_pts[best_c].y += sub_coords[i].y * sub_w[i];
                    weight_sums[best_c] += sub_w[i];
                }
            }

            for (size_t c = 0; c < pts.size(); ++c) {
                if (weight_sums[c] > 1e-9) {
                    pts[c].x = new_pts[c].x / weight_sums[c];
                    pts[c].y = new_pts[c].y / weight_sums[c];
                }
            }
        }
    }

    std::vector<cv::Point2f> filtered_pts;
    for (const auto& p : pts) {
        int xi = std::clamp(int(p.x), 0, w - 1);
        int yi = std::clamp(int(p.y), 0, h - 1);
        if (original_dark[yi * w + xi] >= min_brightness) {
            filtered_pts.push_back(p);
        }
    }
    return filtered_pts;
}


// --- lbg_pfms.cpp ---
std::vector<PFMSetting> makeLbgSettings(std::vector<PFMSetting> settings) {
    for (auto& setting : settings) {
        if (setting.key == "lloyd_iterations") {
            setting.defaultVal = 5;
            setting.value = 5;
        }
    }
    return settings;
}


// --- adaptive_circular_scribbles_pfm.cpp ---
// ---------------------------------------------------------------------------

AdaptiveCircularScribblesPFM::AdaptiveCircularScribblesPFM() {}

std::vector<PFMSetting> AdaptiveCircularScribblesPFM::defineSettings() const {
    auto s = BaseAdaptivePFM::defineSettings();
    s.push_back({"scribble_turns", "Scribble Turns", SettingType::Number, 2.0, SettingValue(), 0.5, 8.0, 0.5, 8.0, 0.5});
    s.push_back({"radius_scale",   "Radius Scale",   SettingType::Number, 3.0, SettingValue(), 0.5, 20.0, 0.5, 20.0, 0.5});
    return s;
}

std::vector<DrawingGeometry> AdaptiveCircularScribblesPFM::_process(const cv::Mat& image) {
    // Apply plotting_resolution scaling
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image;
    }
    auto pts = getSeeds(workImg);
    double turns   = m_settings["scribble_turns"].toDouble();
    double rscale  = m_settings["radius_scale"].toDouble();
    std::vector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        if (i % 50 == 0) emitProgress(float(i) / pts.size(), (int)geoms.size(), "Adaptive scribbles...");
        float cx = pts[i].x, cy = pts[i].y;
        int steps = std::max(12, (int)(turns * 24));
        Path path;
        for (int s = 0; s <= steps; ++s) {
            double t = (double)s / steps * turns * 3.141592653589793 * 2.0;
            path.push_back({cx + std::cos(t) * (float)rscale,
                            cy + std::sin(t) * (float)rscale});
        }
        geoms.push_back(DrawingGeometry{path, 0});
    }
    return geoms;
}


// --- adaptive_dashes_pfm.cpp ---
// ---------------------------------------------------------------------------

std::vector<DrawingGeometry> AdaptiveDashesPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image;
    }
    auto pts = getSeeds(workImg);
    std::vector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        float cx = pts[i].x, cy = pts[i].y;
        float angle  = (float)randUniform(0.0, 3.141592653589793);
        float length = (float)randUniform(3.0, 12.0);
        Path path = {
            {cx, cy},
            {cx + std::cos(angle) * length, cy + std::sin(angle) * length}
        };
        geoms.push_back(DrawingGeometry{path, 0});
    }
    emitProgress(1.0f, (int)geoms.size(), "Adaptive dashes done");
    return geoms;
}


// --- adaptive_diagram_pfm.cpp ---
// ---------------------------------------------------------------------------

std::vector<DrawingGeometry> AdaptiveDiagramPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image;
    }
    auto pts = getSeeds(workImg);
    if (pts.empty()) return {};
    emitProgress(0.5f, 0, "Voronoi facets...");
    cv::Subdiv2D subdiv(cv::Rect(0, 0, workImg.cols, workImg.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<std::vector<cv::Point2f>> facets;
    std::vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);
    std::vector<DrawingGeometry> geoms;
    for (const auto& facet : facets) {
        if (facet.size() < 2) continue;
        Path path;
        for (const auto& pt : facet) path.push_back({pt.x, pt.y});
        path.push_back({facet[0].x, facet[0].y}); // close
        geoms.push_back(DrawingGeometry{path, 0});
    }
    emitProgress(1.0f, (int)geoms.size(), "Adaptive diagram done");
    return geoms;
}


// --- adaptive_letters_pfm.cpp ---


// --- adaptive_pfm.cpp ---
AdaptiveStipplingPFM::AdaptiveStipplingPFM() {
    initSettings();
}

std::vector<PFMSetting> AdaptiveStipplingPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0,  {}, 0.05,2.0,  0.1, 1.0, 0.05 },
        { "random_seed",         "Random Seed",         SettingType::Integer, 42,   {}, 0,  999999, 0,   999999, 1 },
        { "cell_count",          "Cell Count",          SettingType::Integer, 3000, {}, 100,50000,  100, 20000, 100 },
        { "lloyd_iterations",    "Lloyd Iterations",    SettingType::Integer, 3,    {}, 0,  20,     0,   10,     1 },
        { "min_brightness",      "Min Brightness",      SettingType::Percentage, 80.0, {}, 0.0, 100.0, 0.0, 100.0, 1.0 },
        { "dot_radius",          "Dot Radius",          SettingType::Number,  1.0,  {}, 0.2,8.0,   0.2, 5.0,  0.1 },
    };
}

std::vector<DrawingGeometry> AdaptiveStipplingPFM::_process(const cv::Mat& image) {
    // Apply plotting resolution
    cv::Mat workImg;
    double plotRes = get("plotting_resolution").toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image.clone();
    }
    const int W = workImg.cols, H = workImg.rows;

    const int    cellCount     = get("cell_count").toInt();
    const int    lloydIter     = get("lloyd_iterations").toInt();
    const double minBrightPct  = get("min_brightness").toDouble() / 100.0;
    const double dotRadius     = get("dot_radius").toDouble();

    // Build darkness weights
    cv::Mat imgF;
    workImg.convertTo(imgF, CV_32F);
    std::vector<double> weights(W * H);
    double totalW = 0.0;
    const float* fp = imgF.ptr<float>(0);
    for (int i = 0; i < W * H; ++i) {
        double d = std::max(0.0, 255.0 - (double)fp[i]);
        weights[i] = d;
        totalW += d;
    }
    if (totalW < 1.0) return {};

    // Initial sample (weighted)
    std::vector<std::pair<double,double>> points(cellCount);
    for (int i = 0; i < cellCount; ++i) {
        int idx = weightedChoice(weights);
        points[i] = { (double)(idx % W), (double)(idx / W) };
    }

    // Lloyd relaxation (simplified: re-weight toward local mean)
    for (int iter = 0; iter < lloydIter && !isCancelled(); ++iter) {
        // For each point, move slightly toward the darkest nearby pixel
        for (auto& pt : points) {
            int px = (int)pt.first, py = (int)pt.second;
            // Sample a small neighborhood
            double bestW = -1.0;
            double bx = pt.first, by = pt.second;
            for (int dy = -5; dy <= 5; ++dy)
                for (int dx = -5; dx <= 5; ++dx) {
                    int nx = px + dx, ny = py + dy;
                    if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;
                    double w = weights[ny * W + nx];
                    if (w > bestW) { bestW = w; bx = nx; by = ny; }
                }
            pt = { bx, by };
        }
    }

    // Filter by brightness (min_brightness removes dots in very bright areas)
    double minBrightAbs = minBrightPct * 255.0;

    std::vector<DrawingGeometry> geometries;
    geometries.reserve(cellCount);

    for (int i = 0; i < (int)points.size() && !isCancelled(); ++i) {
        double cx = points[i].first, cy = points[i].second;
        int xi = (int)cx, yi = (int)cy;
        if (xi < 0 || xi >= W || yi < 0 || yi >= H) continue;
        double bright = fp[yi * W + xi];
        if (bright > minBrightAbs) continue;  // skip bright areas

        Path circle = generate_circle(cx, cy, dotRadius, 8);
        geometries.push_back(DrawingGeometry{ circle, 0 });

        if (i % 500 == 0)
            emitProgress((float)i / points.size(), geometries.size(), "Adaptive stippling...");
    }
    return geometries;
}


// --- adaptive_shapes_pfm.cpp ---
// ---------------------------------------------------------------------------

AdaptiveShapesPFM::AdaptiveShapesPFM() {}

std::vector<PFMSetting> AdaptiveShapesPFM::defineSettings() const {
    auto s = BaseAdaptivePFM::defineSettings();
    s.push_back({"shape_size", "Shape Size", SettingType::Number, 4.0, SettingValue(), 1.0, 30.0, 1.0, 30.0, 0.5});
    return s;
}

std::vector<DrawingGeometry> AdaptiveShapesPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image;
    }
    auto pts = getSeeds(workImg);
    float size = (float)m_settings["shape_size"].toDouble();
    std::vector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        if (i % 50 == 0) emitProgress(float(i) / pts.size(), (int)geoms.size(), "Adaptive shapes...");
        float cx = pts[i].x, cy = pts[i].y;
        Path path;
        int shapeType = i % 4;
        switch (shapeType) {
            case 0: path = generate_circle(cx, cy, size, 12); break;
            case 1: path = generate_polygon(cx, cy, size, 4, 45); break;
            case 2: path = generate_polygon(cx, cy, size, 3, 30); break;
            case 3: path = generate_polygon(cx, cy, size, 6, 0); break;
        }
        geoms.push_back(DrawingGeometry{path, 0});
    }
    return geoms;
}


// --- adaptive_tree_pfm.cpp ---
// ---------------------------------------------------------------------------

std::vector<DrawingGeometry> AdaptiveTreePFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image;
    }
    auto pts = getSeeds(workImg);
    if (pts.empty()) return {};
    emitProgress(0.3f, 0, "Building MST...");
    cv::Subdiv2D subdiv(cv::Rect(0, 0, workImg.cols, workImg.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<cv::Vec4f> edges;
    subdiv.getEdgeList(edges);

    std::map<std::pair<float,float>, int> pt2idx;
    for (size_t i = 0; i < pts.size(); ++i) pt2idx[{pts[i].x, pts[i].y}] = i;

    std::vector<std::vector<std::pair<int,float>>> adj(pts.size());
    for (const auto& e : edges) {
        auto it1 = pt2idx.find({e[0], e[1]});
        auto it2 = pt2idx.find({e[2], e[3]});
        if (it1 != pt2idx.end() && it2 != pt2idx.end()) {
            int u = it1->second, v = it2->second;
            float d = std::hypot(e[0]-e[2], e[1]-e[3]);
            adj[u].push_back({v, d});
            adj[v].push_back({u, d});
        }
    }
    std::vector<bool> in_mst(pts.size(), false);
    std::vector<float> min_w(pts.size(), 1e12f);
    std::vector<int> parent(pts.size(), -1);
    std::priority_queue<std::pair<float,int>, std::vector<std::pair<float,int>>,
                        std::greater<std::pair<float,int>>> pq;
    pq.push({0.f, 0});
    min_w[0] = 0.f;
    std::vector<DrawingGeometry> geoms;
    while (!pq.empty()) {
        if (isCancelled()) return {};
        int u = pq.top().second; pq.pop();
        if (in_mst[u]) continue;
        in_mst[u] = true;
        if (parent[u] != -1) {
            geoms.push_back(DrawingGeometry{
                {{pts[parent[u]].x, pts[parent[u]].y}, {pts[u].x, pts[u].y}}, 0});
        }
        for (const auto& edge : adj[u]) {
            int v = edge.first; float w = edge.second;
            if (!in_mst[v] && w < min_w[v]) { min_w[v] = w; parent[v] = u; pq.push({w, v}); }
        }
    }
    emitProgress(1.0f, (int)geoms.size(), "Adaptive tree done");
    return geoms;
}


// --- adaptive_triangulation_pfm.cpp ---
// ---------------------------------------------------------------------------

std::vector<DrawingGeometry> AdaptiveTriangulationPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image;
    }
    auto pts = getSeeds(workImg);
    if (pts.empty()) return {};
    emitProgress(0.5f, 0, "Delaunay triangulation...");
    cv::Subdiv2D subdiv(cv::Rect(0, 0, workImg.cols, workImg.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<cv::Vec4f> edges;
    subdiv.getEdgeList(edges);
    std::vector<DrawingGeometry> geoms;
    for (const auto& e : edges) {
        geoms.push_back(DrawingGeometry{{{e[0], e[1]}, {e[2], e[3]}}, 0});
    }
    emitProgress(1.0f, (int)geoms.size(), "Triangulation done");
    return geoms;
}


// --- adaptive_tsppfm.cpp ---
inline Path solve_tsp_adaptive(const std::vector<cv::Point2f>& points,
                                std::function<bool()> isCancelledFn) {
    if (points.empty()) return {};
    std::vector<bool> visited(points.size(), false);
    Path path;
    path.reserve(points.size());
    int current = 0;
    visited[0] = true;
    path.push_back({points[0].x, points[0].y});
    int remaining = (int)points.size() - 1;
    while (remaining > 0) {
        if (isCancelledFn()) return {};
        int best_i = -1;
        float best_d = 1e12f;
        for (size_t i = 0; i < points.size(); ++i) {
            if (!visited[i]) {
                float dx = points[i].x - points[current].x;
                float dy = points[i].y - points[current].y;
                float d = dx*dx + dy*dy;
                if (d < best_d) { best_d = d; best_i = (int)i; }
            }
        }
        if (best_i == -1) break;
        visited[best_i] = true;
        path.push_back({points[best_i].x, points[best_i].y});
        current = best_i;
        remaining--;
    }
    return path;
}

// ---------------------------------------------------------------------------

std::vector<DrawingGeometry> AdaptiveTSPPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image;
    }
    auto pts = getSeeds(workImg);
    if (pts.size() < 2) return {};
    emitProgress(0.3f, 0, "TSP solving...");
    Path path = solve_tsp_adaptive(pts, [this](){ return isCancelled(); });
    if (path.size() < 2) return {};
    return {DrawingGeometry{path, 0}};
}


// --- ambient_flow_pfm.cpp ---
Path trace_streamline_local(const cv::Mat& fx, const cv::Mat& fy, float x, float y, int max_len, float step_size) {
        Path path;
        int w = fx.cols;
        int h = fx.rows;
        for (int i = 0; i < max_len; ++i) {
            int xi = std::clamp(int(x), 0, w - 1);
            int yi = std::clamp(int(y), 0, h - 1);
            path.push_back({x, y});
            
            float vx = fx.at<float>(yi, xi);
            float vy = fy.at<float>(yi, xi);
            float mag = std::hypot(vx, vy);
            if (mag < 1e-6f) break;
            
            x += (vx / mag) * step_size;
            y += (vy / mag) * step_size;
            if (x < 0 || x >= w || y < 0 || y >= h) break;
        }
        return path;
    }

AmbientFlowPFM::AmbientFlowPFM() {
    initSettings();
}

std::vector<PFMSetting> AmbientFlowPFM::defineSettings() const {
    return {
        {"lines", "Lines", SettingType::Integer, 200, SettingValue(), 20, 2000, 20, 2000, 10},
        {"length", "Max Length", SettingType::Integer, 150, SettingValue(), 20, 500, 20, 500, 10}
    };
}

std::vector<DrawingGeometry> AmbientFlowPFM::_process(const cv::Mat& image) {
    int count = m_settings["lines"].toInt();
    int max_len = m_settings["length"].toInt();
    
    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    cv::GaussianBlur(gx, gx, cv::Size(9, 9), 0);
    cv::GaussianBlur(gy, gy, cv::Size(9, 9), 0);
    
    std::vector<DrawingGeometry> geoms;
    for (int i = 0; i < count; ++i) {
        if (isCancelled()) break;
        if (i % 20 == 0) emitProgress(float(i) / count, geoms.size(), "Ambient Flow...");
        float x = randUniform(0, image.cols);
        float y = randUniform(0, image.rows);
        Path path = trace_streamline_local(gx, gy, x, y, max_len, 1.2f);
        if (path.size() >= 2) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}


// --- contour_paths_pfm.cpp ---
// -------------------------------------------------------------------------
ContourPathsPFM::ContourPathsPFM() {
    initSettings();
}

std::vector<PFMSetting> ContourPathsPFM::defineSettings() const {
    return {
        {"threshold", "Threshold", SettingType::Integer, 127, SettingValue(), 0, 255, 0, 255, 1},
        {"epsilon", "Simplify Epsilon", SettingType::Number, 1.5, SettingValue(), 0.0, 10.0, 0.0, 10.0, 0.1}
    };
}

std::vector<DrawingGeometry> ContourPathsPFM::_process(const cv::Mat& image) {
    int threshold = m_settings["threshold"].toInt();
    double epsilon = m_settings["epsilon"].toDouble();

    cv::Mat img8u;
    image.convertTo(img8u, CV_8UC1);
    
    cv::Mat binary;
    cv::threshold(img8u, binary, threshold, 255, cv::THRESH_BINARY_INV);
    
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(binary, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    
    std::vector<DrawingGeometry> geoms;
    for (size_t i = 0; i < contours.size(); ++i) {
        if (isCancelled()) break;
        if (i % 100 == 0) emitProgress(float(i) / contours.size(), geoms.size(), "Contours...");
        
        const auto& cnt = contours[i];
        if (cnt.size() < 2) continue;
        
        std::vector<cv::Point> approx;
        cv::approxPolyDP(cnt, approx, epsilon, true);
        
        Path path;
        for (const auto& p : approx) {
            path.push_back({float(p.x), float(p.y)});
        }
        
        if (path.size() >= 2) {
            DrawingGeometry dg; dg.path = path;
            geoms.push_back(dg);
        }
    }
    return geoms;
}


// --- edge_shading_pfm.cpp ---
// -------------------------------------------------------------------------
EdgeShadingPFM::EdgeShadingPFM() {
    initSettings();
}

std::vector<PFMSetting> EdgeShadingPFM::defineSettings() const {
    return {
        {"canny1", "Canny Low", SettingType::Integer, 50, SettingValue(), 0, 255, 0, 255, 5},
        {"canny2", "Canny High", SettingType::Integer, 150, SettingValue(), 0, 255, 0, 255, 5},
        {"spacing", "Hatch Spacing", SettingType::Number, 4.0, SettingValue(), 1.0, 30.0, 1.0, 30.0, 0.5}
    };
}

std::vector<DrawingGeometry> EdgeShadingPFM::_process(const cv::Mat& image) {
    int canny1 = m_settings["canny1"].toInt();
    int canny2 = m_settings["canny2"].toInt();
    float spacing = m_settings["spacing"].toDouble();
    
    cv::Mat img8u;
    image.convertTo(img8u, CV_8UC1);
    
    cv::Mat edges;
    cv::Canny(img8u, edges, canny1, canny2);
    
    cv::Mat combined;
    cv::Mat edgesF;
    edges.convertTo(edgesF, CV_32F);
    cv::addWeighted(image, 0.5, edgesF, 0.5, 0.0, combined);
    
    std::vector<DrawingGeometry> geoms;
    int w = combined.cols;
    int h = combined.rows;
    float diag_len = std::hypot(w, h);
    int num_lines = int(diag_len / std::max(1.0f, spacing));
    
    float rad = 45.0f * 3.141592653589793 / 180.0f;
    float dx = std::cos(rad);
    float dy = std::sin(rad);
    float depth = 1.0f;
    float thresh = (80.0f / 100.0f) * 255.0f;
    
    for (int i = 0; i < num_lines; ++i) {
        if (isCancelled()) break;
        if (i % 20 == 0) emitProgress(float(i) / num_lines, geoms.size(), "Edge Shading...");
        
        float offset = (i - num_lines / 2.0f) * spacing;
        float cx = w / 2.0f + offset * -dy;
        float cy = h / 2.0f + offset * dx;
        float x1 = cx - dx * diag_len;
        float y1 = cy - dy * diag_len;
        float x2 = cx + dx * diag_len;
        float y2 = cy + dy * diag_len;
        
        Path path;
        int steps = std::max(1, int(diag_len));
        bool up = true;
        for (int step = 0; step < steps; step += 3) {
            float px = x1 + (x2 - x1) * (float(step) / steps);
            float py = y1 + (y2 - y1) * (float(step) / steps);
            
            if (px >= 0 && px < w && py >= 0 && py < h) {
                float val = combined.at<float>(int(py), int(px));
                float brightness = val / 255.0f;
                if ((255.0f - val) > thresh) {
                    float local_depth = depth * (1.0f - brightness);
                    float wx = px + (up ? -dy : dy) * local_depth;
                    float wy = py + (up ? dx : -dx) * local_depth;
                    path.push_back({wx, wy});
                    up = !up;
                } else {
                    if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
                    path.clear();
                }
            }
        }
        if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}


// --- grid_dashes_pfm.cpp ---
std::vector<DrawingGeometry> GridDashesPFM::_process(const cv::Mat& image) {
    int cols = m_settings["cols"].toInt();
    int rows = m_settings["rows"].toInt();
    float thresh = m_settings["threshold"].toDouble();
    float cw = float(image.cols) / cols;
    float ch = float(image.rows) / rows;
    
    std::vector<DrawingGeometry> geoms;
    for (int r = 0; r < rows; ++r) {
        if (isCancelled()) break;
        emitProgress(float(r) / rows, geoms.size(), "Grid Dashes...");
        for (int c = 0; c < cols; ++c) {
            cv::Rect roi(int(c * cw), int(r * ch), int(cw), int(ch));
            if (roi.x + roi.width > image.cols) roi.width = image.cols - roi.x;
            if (roi.y + roi.height > image.rows) roi.height = image.rows - roi.y;
            if (roi.width <= 0 || roi.height <= 0) continue;
            
            double mean = cv::mean(image(roi))[0];
            float dark = 255.0f - mean;
            if (dark >= thresh) {
                float cx = c * cw;
                float cy = r * ch + ch / 2.0f;
                DrawingGeometry dg;
                dg.path = {{cx + cw*0.2f, cy}, {cx + cw*0.8f, cy}};
                geoms.push_back(dg);
            }
        }
    }
    return geoms;
}


// --- grid_letters_pfm.cpp ---
inline std::map<char, std::vector<std::pair<float, float>>> GRID_LETTER_GLYPHS = {
    {'A', {{0.1, 1}, {0.5, 0}, {0.9, 1}, {0.7, 0.55}, {0.3, 0.55}}},
    {'B', {{0.2, 0}, {0.2, 1}, {0.6, 1}, {0.8, 0.85}, {0.6, 0.5}, {0.8, 0.15}, {0.6, 0}, {0.2, 0}}},
    {'C', {{0.9, 0.15}, {0.6, 0}, {0.3, 0}, {0.1, 0.3}, {0.1, 0.7}, {0.3, 1}, {0.6, 1}, {0.9, 0.85}}},
    {'D', {{0.2, 0}, {0.2, 1}, {0.55, 1}, {0.85, 0.75}, {0.85, 0.25}, {0.55, 0}, {0.2, 0}}},
    {'E', {{0.8, 0}, {0.2, 0}, {0.2, 1}, {0.8, 1}, {0.2, 0.5}, {0.65, 0.5}}},
    {'F', {{0.2, 0}, {0.2, 1}, {0.8, 1}, {0.2, 0.5}, {0.7, 0.5}}},
    {'G', {{0.9, 0.2}, {0.6, 0}, {0.3, 0.05}, {0.1, 0.4}, {0.1, 0.7}, {0.35, 1}, {0.75, 0.9}, {0.75, 0.55}, {0.5, 0.55}}},
    {'H', {{0.2, 0}, {0.2, 1}, {0.2, 0.5}, {0.8, 0.5}, {0.8, 1}, {0.8, 0}}},
    {'I', {{0.35, 0}, {0.65, 0}, {0.5, 0}, {0.5, 1}, {0.35, 1}, {0.65, 1}}},
    {'J', {{0.7, 0}, {0.3, 0}, {0.3, 0.8}, {0.5, 1}, {0.75, 0.85}}},
    {'K', {{0.2, 0}, {0.2, 1}, {0.2, 0.5}, {0.85, 1}, {0.25, 0.5}, {0.85, 0}}},
    {'L', {{0.2, 1}, {0.2, 0}, {0.85, 0}}},
    {'M', {{0.1, 0}, {0.1, 1}, {0.5, 0.5}, {0.9, 1}, {0.9, 0}}},
    {'N', {{0.15, 0}, {0.15, 1}, {0.85, 0}, {0.85, 1}}},
    {'O', {{0.5, 0}, {0.15, 0.15}, {0, 0.5}, {0.15, 0.85}, {0.5, 1}, {0.85, 0.85}, {1, 0.5}, {0.85, 0.15}, {0.5, 0}}},
    {'P', {{0.2, 0}, {0.2, 1}, {0.65, 1}, {0.85, 0.8}, {0.65, 0.5}, {0.2, 0.5}}},
    {'Q', {{0.5, 0}, {0.15, 0.15}, {0, 0.5}, {0.2, 0.85}, {0.5, 1}, {0.85, 0.85}, {1, 0.5}, {0.85, 0.15}, {0.5, 0}, {0.75, 0.25}, {1, 0}}},
    {'R', {{0.2, 0}, {0.2, 1}, {0.65, 1}, {0.85, 0.8}, {0.65, 0.5}, {0.2, 0.5}, {0.85, 0}}},
    {'S', {{0.85, 0.85}, {0.55, 1}, {0.25, 0.9}, {0.15, 0.65}, {0.45, 0.5}, {0.75, 0.35}, {0.85, 0.15}, {0.55, 0}, {0.25, 0.1}}},
    {'T', {{0, 1}, {1, 1}, {0.5, 1}, {0.5, 0}}},
    {'U', {{0.15, 1}, {0.15, 0.25}, {0.35, 0}, {0.65, 0}, {0.85, 0.25}, {0.85, 1}}},
    {'V', {{0, 1}, {0.5, 0}, {1, 1}}},
    {'W', {{0, 1}, {0.25, 0}, {0.5, 0.6}, {0.75, 0}, {1, 1}}},
    {'X', {{0, 0}, {1, 1}, {0, 1}, {1, 0}}},
    {'Y', {{0, 1}, {0.5, 0.45}, {1, 1}, {0.5, 0.45}, {0.5, 0}}},
    {'Z', {{0.1, 1}, {0.9, 1}, {0.1, 0}, {0.9, 0}}}
};

inline Path grid_letter_glyph_path(char letter, float cx, float cy, float size) {
    if (GRID_LETTER_GLYPHS.find(letter) == GRID_LETTER_GLYPHS.end()) letter = 'O';
    Path p;
    for (const auto& pt : GRID_LETTER_GLYPHS[letter]) {
        p.push_back({cx + (pt.first - 0.5f) * size, cy + (0.5f - pt.second) * size});
    }
    return p;
}

static char grid_letter_for_brightness(float darkness) {
    int idx = int(std::clamp(darkness / 255.0f, 0.0f, 0.999f) * 26.0f);
    const char* letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    return letters[idx];
}

std::vector<DrawingGeometry> GridLettersPFM::_process(const cv::Mat& image) {
    int cols = m_settings["cols"].toInt();
    int rows = m_settings["rows"].toInt();
    float thresh = m_settings["threshold"].toDouble();
    float cw = float(image.cols) / cols;
    float ch = float(image.rows) / rows;
    
    std::vector<DrawingGeometry> geoms;
    for (int r = 0; r < rows; ++r) {
        if (isCancelled()) break;
        emitProgress(float(r) / rows, geoms.size(), "Grid Letters...");
        for (int c = 0; c < cols; ++c) {
            cv::Rect roi(int(c * cw), int(r * ch), int(cw), int(ch));
            if (roi.x + roi.width > image.cols) roi.width = image.cols - roi.x;
            if (roi.y + roi.height > image.rows) roi.height = image.rows - roi.y;
            if (roi.width <= 0 || roi.height <= 0) continue;
            
            double mean = cv::mean(image(roi))[0];
            float dark = 255.0f - mean;
            if (dark >= thresh) {
                float cx = c * cw + cw / 2.0f;
                float cy = r * ch + ch / 2.0f;
                char letter = grid_letter_for_brightness(dark);
                DrawingGeometry dg;
                dg.path = grid_letter_glyph_path(letter, cx, cy, std::min(cw, ch) * 0.75f);
                geoms.push_back(dg);
            }
        }
    }
    return geoms;
}


// --- grid_shapes_pfm.cpp ---
std::vector<DrawingGeometry> GridShapesPFM::_process(const cv::Mat& image) {
    int cols = m_settings["cols"].toInt();
    int rows = m_settings["rows"].toInt();
    float thresh = m_settings["threshold"].toDouble();
    float cw = float(image.cols) / cols;
    float ch = float(image.rows) / rows;
    
    std::vector<DrawingGeometry> geoms;
    for (int r = 0; r < rows; ++r) {
        if (isCancelled()) break;
        emitProgress(float(r) / rows, geoms.size(), "Grid Shapes...");
        for (int c = 0; c < cols; ++c) {
            cv::Rect roi(int(c * cw), int(r * ch), int(cw), int(ch));
            if (roi.x + roi.width > image.cols) roi.width = image.cols - roi.x;
            if (roi.y + roi.height > image.rows) roi.height = image.rows - roi.y;
            if (roi.width <= 0 || roi.height <= 0) continue;
            
            double mean = cv::mean(image(roi))[0];
            float dark = 255.0f - mean;
            if (dark >= thresh) {
                float cx = c * cw + cw / 2.0f;
                float cy = r * ch + ch / 2.0f;
                DrawingGeometry dg;
                dg.path = generate_polygon(cx, cy, std::min(cw, ch) * 0.4f, 4 + (r + c) % 3);
                geoms.push_back(dg);
            }
        }
    }
    return geoms;
}


// --- hatch3_way_pfm.cpp ---


// --- hatch4_way_pfm.cpp ---


// --- hatch5_way_pfm.cpp ---


// --- hatch6_way_pfm.cpp ---


// --- hatch_circular_scribbles_pfm.cpp ---
HatchCircularScribblesPFM::HatchCircularScribblesPFM() {
    initSettings();
}

std::vector<PFMSetting> HatchCircularScribblesPFM::defineSettings() const {
    return {
        {"spacing", "Ring Spacing", SettingType::Number, 6.0, SettingValue(), 2.0, 40.0, 2.0, 40.0, 0.5},
        {"threshold", "Darkness Threshold", SettingType::Percentage, 50.0, SettingValue(), 0.0, 100.0, 0.0, 100.0, 1.0},
        {"turns", "Turns per Ring", SettingType::Number, 1.5, SettingValue(), 0.5, 4.0, 0.5, 4.0, 0.1}
    };
}

std::vector<DrawingGeometry> HatchCircularScribblesPFM::_process(const cv::Mat& image) {
    float spacing = m_settings["spacing"].toDouble();
    float thresh = (m_settings["threshold"].toDouble() / 100.0f) * 255.0f;
    float turns = m_settings["turns"].toDouble();
    
    std::vector<DrawingGeometry> geoms;
    float cx = image.cols / 2.0f;
    float cy = image.rows / 2.0f;
    float max_r = std::hypot(cx, cy);
    int num_rings = int(max_r / spacing);
    
    for (int r_idx = 1; r_idx <= num_rings; ++r_idx) {
        if (isCancelled()) break;
        emitProgress(float(r_idx) / num_rings, geoms.size(), "Circular Scribbles...");
        
        float base_r = r_idx * spacing;
        Path path;
        int steps = std::max(36, int(2.0f * 3.141592653589793 * base_r * turns));
        for (int step = 0; step < steps; ++step) {
            float theta = (float(step) / steps) * 2.0f * 3.141592653589793 * turns;
            float r = base_r + std::sin(theta * (base_r / 2.0f)) * (spacing * 0.8f);
            
            float px = cx + r * std::cos(theta);
            float py = cy + r * std::sin(theta);
            
            if (px >= 0 && px < image.cols && py >= 0 && py < image.rows) {
                if ((255.0f - image.at<float>(int(py), int(px))) > thresh) {
                    path.push_back({px, py});
                } else {
                    if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
                    path.clear();
                }
            }
        }
        if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}


// --- hatch_cross_pfm.cpp ---


// --- hatch_diagonal1_pfm.cpp ---


// --- hatch_diagonal2_pfm.cpp ---


// --- hatch_grid_pfm.cpp ---


// --- hatch_horizontal_pfm.cpp ---


// --- hatch_lines.cpp ---
HatchLinesPFM::HatchLinesPFM() {
    initSettings();
}

std::vector<PFMSetting> HatchLinesPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution",  SettingType::Number,     1.0,  {}, 0.05, 2.0,  0.1,  1.0,  0.05 },
        { "random_seed",         "Random Seed",          SettingType::Integer,    42,   {}, 0,    999999,0,    999999,1 },
        { "angle1",              "Angle 1",              SettingType::Number,     45.0, {}, -90.0,90.0,-90.0, 90.0, 5.0 },
        { "angle2",              "Angle 2",              SettingType::Number,    -45.0, {}, -90.0,90.0,-90.0, 90.0, 5.0 },
        { "spacing",             "Spacing",              SettingType::Number,     5.0,  {}, 1.0,  50.0, 1.0,  30.0, 0.5 },
        { "threshold",           "Darkness Threshold",   SettingType::Percentage, 50.0, {}, 0.0,  100.0,0.0,  100.0,1.0 },
    };
}

std::vector<DrawingGeometry> HatchLinesPFM::_process(const cv::Mat& image) {
    const int h = image.rows, w = image.cols;
    const double spacing = get("spacing").toDouble();
    const double thresh  = get("threshold").toDouble() / 100.0 * 255.0;

    cv::Mat imgF;
    image.convertTo(imgF, CV_32F);

    std::vector<DrawingGeometry> geometries;

    for (double angleDeg : { get("angle1").toDouble(), get("angle2").toDouble() }) {
        if (isCancelled()) break;

        const double rad = angleDeg * 3.141592653589793 / 180.0;
        const double dx = std::cos(rad), dy = std::sin(rad);
        const double diagLen = std::hypot(w, h);
        const int numLines = (int)(diagLen / spacing);

        for (int i = 0; i < numLines && !isCancelled(); ++i) {
            double offset = (i - numLines / 2.0) * spacing;
            double cx0 = w / 2.0 + offset * (-dy);
            double cy0 = h / 2.0 + offset * dx;

            double x1 = cx0 - dx * diagLen, y1 = cy0 - dy * diagLen;
            double x2 = cx0 + dx * diagLen, y2 = cy0 + dy * diagLen;

            Path path;
            int steps = (int)diagLen;
            for (int s = 0; s < steps; ++s) {
                double t  = (double)s / steps;
                double px = x1 + (x2 - x1) * t;
                double py = y1 + (y2 - y1) * t;
                if (px < 0 || px >= w || py < 0 || py >= h) {
                    if (path.size() > 1) geometries.push_back(DrawingGeometry{ path, 0 });
                    path.clear(); continue;
                }
                float b = 255.0f - imgF.at<float>((int)py, (int)px);
                if (b > thresh) {
                    path.push_back({ px, py });
                } else {
                    if (path.size() > 1) geometries.push_back(DrawingGeometry{ path, 0 });
                    path.clear();
                }
            }
            if (path.size() > 1) geometries.push_back(DrawingGeometry{ path, 0 });

            emitProgress((float)i / numLines, geometries.size(), "Hatching...");
        }
    }
    return geometries;
}


// --- hatch_sawtooth_pfm.cpp ---
HatchSawtoothPFM::HatchSawtoothPFM() {
    initSettings();
}

std::vector<PFMSetting> HatchSawtoothPFM::defineSettings() const {
    return {
        {"angle", "Angle", SettingType::Number, 45.0, SettingValue(), -90.0, 90.0, -90.0, 90.0, 5.0},
        {"spacing", "Spacing", SettingType::Number, 5.0, SettingValue(), 1.0, 50.0, 1.0, 50.0, 0.5},
        {"threshold", "Darkness Threshold", SettingType::Percentage, 50.0, SettingValue(), 0.0, 100.0, 0.0, 100.0, 1.0},
        {"tooth_depth", "Tooth Depth", SettingType::Number, 2.0, SettingValue(), 0.5, 15.0, 0.5, 15.0, 0.5}
    };
}

std::vector<DrawingGeometry> HatchSawtoothPFM::_process(const cv::Mat& image) {
    float angle = m_settings["angle"].toDouble();
    float spacing = m_settings["spacing"].toDouble();
    float thresh = (m_settings["threshold"].toDouble() / 100.0f) * 255.0f;
    float depth = m_settings["tooth_depth"].toDouble();
    
    std::vector<DrawingGeometry> geoms;
    int w = image.cols;
    int h = image.rows;
    float diag_len = std::hypot(w, h);
    int num_lines = int(diag_len / std::max(1.0f, spacing));
    
    float rad = angle * 3.141592653589793 / 180.0f;
    float dx = std::cos(rad);
    float dy = std::sin(rad);
    
    for (int i = 0; i < num_lines; ++i) {
        if (isCancelled()) break;
        if (i % 20 == 0) emitProgress(float(i) / num_lines, geoms.size(), "Sawtooth Hatching...");
        
        float offset = (i - num_lines / 2.0f) * spacing;
        float cx = w / 2.0f + offset * -dy;
        float cy = h / 2.0f + offset * dx;
        float x1 = cx - dx * diag_len;
        float y1 = cy - dy * diag_len;
        float x2 = cx + dx * diag_len;
        float y2 = cy + dy * diag_len;
        
        Path path;
        int steps = std::max(1, int(diag_len));
        bool up = true;
        for (int step = 0; step < steps; step += 3) {
            float px = x1 + (x2 - x1) * (float(step) / steps);
            float py = y1 + (y2 - y1) * (float(step) / steps);
            
            if (px >= 0 && px < w && py >= 0 && py < h) {
                float brightness = image.at<float>(int(py), int(px)) / 255.0f;
                if ((255.0f - image.at<float>(int(py), int(px))) > thresh) {
                    float local_depth = depth * (1.0f - brightness);
                    float wx = px + (up ? -dy : dy) * local_depth;
                    float wy = py + (up ? dx : -dx) * local_depth;
                    path.push_back({wx, wy});
                    up = !up;
                } else {
                    if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
                    path.clear();
                }
            }
        }
        if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}


// --- hatch_vertical_pfm.cpp ---


// --- labyrinth_classic_pfm.cpp ---
LabyrinthClassicPFM::LabyrinthClassicPFM() {
    initSettings();
}

std::vector<PFMSetting> LabyrinthClassicPFM::defineSettings() const {
    return {
        {"spacing", "Spiral Spacing", SettingType::Number, 10.0, SettingValue(), 2.0, 50.0, 2.0, 50.0, 1.0},
        {"wobble", "Wobble Factor", SettingType::Number, 0.0, SettingValue(), 0.0, 10.0, 0.0, 10.0, 0.5}
    };
}

std::vector<DrawingGeometry> LabyrinthClassicPFM::_process(const cv::Mat& image) {
    float spacing = m_settings["spacing"].toDouble();
    float wobble = m_settings["wobble"].toDouble();
    float cx = image.cols / 2.0f;
    float cy = image.rows / 2.0f;
    float max_r = std::hypot(cx, cy);
    float theta = 0.0f;
    float b = spacing / (2.0f * 3.141592653589793);
    
    std::vector<DrawingGeometry> geoms;
    Path path;
    int i = 0;
    while (true) {
        if (isCancelled()) break;
        float r = b * theta;
        if (r > max_r) break;
        
        float x = cx + r * std::cos(theta);
        float y = cy + r * std::sin(theta);
        int ix = int(x), iy = int(y);
        
        if (ix >= 0 && ix < image.cols && iy >= 0 && iy < image.rows) {
            float brightness = image.at<float>(iy, ix) / 255.0f;
            float wx = x + (std::cos(theta * 10.0f) * wobble * (1.0f - brightness));
            float wy = y + (std::sin(theta * 10.0f) * wobble * (1.0f - brightness));
            path.push_back({wx, wy});
        } else {
            if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
            path.clear();
        }
        float step = std::max(0.01f, std::min(0.5f, 2.0f / std::max(1.0f, r)));
        theta += step;
        if (i++ % 5000 == 0) emitProgress(r / max_r, geoms.size(), "Labyrinth...");
    }
    if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    return geoms;
}


// --- labyrinth_roman_pfm.cpp ---


// --- layers_pfm.cpp ---
// ---------------------------------------------------------------------------

LayersPFM::LayersPFM() {
    initSettings();
}

std::vector<PFMSetting> LayersPFM::defineSettings() const {
    return {
        {"plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0, SettingValue(), 0.1, 2.0, 0.1, 2.0, 0.05},
        {"random_seed",         "Random Seed",         SettingType::Integer, 42,  SettingValue(), 0, 999999, 0, 999999, 1},
        {"num_layers",          "Number of Layers",    SettingType::Integer, 4,   SettingValue(), 2, 8, 2, 8, 1},
        {"line_density",        "Line Density (%)",    SettingType::Percentage, 40.0, SettingValue(), 0, 100, 10, 100, 5},
    };
}

std::vector<DrawingGeometry> LayersPFM::_process(const cv::Mat& image) {
    // Optionally resize to plotting resolution
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image.clone();
    }

    int n = m_settings["num_layers"].toInt();
    double lineDensity = m_settings["line_density"].toDouble();
    std::vector<DrawingGeometry> geoms;

    // Linearly spaced brightness thresholds
    float step = 200.0f / n;
    for (int layer = 0; layer < n && !isCancelled(); ++layer) {
        float lo = 30.0f + layer * step;
        float hi = lo + step;

        // Build a masked image: only pixels in [lo, hi) brightness range are drawn
        cv::Mat layerImg = cv::Mat::ones(workImg.size(), CV_8UC1) * 255;
        for (int y = 0; y < workImg.rows; ++y) {
            const uchar* src = workImg.ptr<uchar>(y);
            uchar* dst = layerImg.ptr<uchar>(y);
            for (int x = 0; x < workImg.cols; ++x) {
                float v = (float)src[x];
                if (v >= lo && v < hi) dst[x] = src[x];
                // else stays white (255)
            }
        }

        // Run SketchLinesPFM on this layer
        SketchLinesPFM sketch;
        sketch.set("line_density", lineDensity);
        sketch.set("plotting_resolution", 1.0);

        auto layerGeoms = sketch.process(layerImg);
        for (auto& g : layerGeoms) {
            g.penIndex   = layer;
            g.groupIndex = layer;
            geoms.push_back(g);
        }
        emitProgress(float(layer + 1) / n, geoms.size(),
                     "" /*progress msg*/);
    }
    return geoms;
}


// --- lbg_circular_scribbles_pfm.cpp ---
std::vector<PFMSetting> LBGCircularScribblesPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveCircularScribblesPFM().settingsList());
}


// --- lbg_dashes_pfm.cpp ---
std::vector<PFMSetting> LBGDashesPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveDashesPFM().settingsList());
}


// --- lbg_diagram_pfm.cpp ---
std::vector<PFMSetting> LBGDiagramPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveDiagramPFM().settingsList());
}


// --- lbg_letters_pfm.cpp ---
// -------------------------------------------------------------------------
LBGLettersPFM::LBGLettersPFM() {}

int LBGLettersPFM::getLloydIters() const { 
    int iters = get("lloyd_iterations").toInt();
    return iters > 0 ? iters : 5; 
}

std::vector<PFMSetting> LBGLettersPFM::defineSettings() const {
    auto s = _LettersBasePFM::defineSettings();
    for (auto& setting : s) {
        if (setting.key == "lloyd_iterations") {
            setting.defaultVal = 5;
            setting.value = 5;
        }
    }
    return s;
}


// --- lbg_shapes_pfm.cpp ---
std::vector<PFMSetting> LBGShapesPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveShapesPFM().settingsList());
}


// --- lbg_stippling_pfm.cpp ---
std::vector<PFMSetting> LBGStipplingPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveStipplingPFM().settingsList());
}


// --- lbg_tree_pfm.cpp ---
std::vector<PFMSetting> LBGTreePFM::defineSettings() const {
    return makeLbgSettings(AdaptiveTreePFM().settingsList());
}


// --- lbg_triangulation_pfm.cpp ---
std::vector<PFMSetting> LBGTriangulationPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveTriangulationPFM().settingsList());
}


// --- lbgtsppfm.cpp ---
std::vector<PFMSetting> LBGTSPPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveTSPPFM().settingsList());
}


// --- maze_circle_pfm.cpp ---
std::pair<float, float> MazeCirclePFM::transformPoint(float x, float y) {
    float r = y + 10.0f;
    float theta = x / 10.0f;
    return {r * std::cos(theta), r * std::sin(theta)};
}


// --- maze_hex_pfm.cpp ---
std::pair<float, float> MazeHexPFM::transformPoint(float x, float y) {
    float offset = (int(y / 10.0) % 2 != 0) ? 5.0f : 0.0f;
    return {x + offset, y * std::sqrt(3.0f) / 2.0f};
}


// --- maze_hilbert_pfm.cpp ---
inline void hilbert_curve(float x0, float y0, float xi, float xj, float yi, float yj, int n, Path& pts) {
    if (n <= 0) { pts.push_back({x0 + (xi + yi) / 2.0f, y0 + (xj + yj) / 2.0f}); return; }
    hilbert_curve(x0, y0, yi/2, yj/2, xi/2, xj/2, n - 1, pts);
    hilbert_curve(x0 + xi/2, y0 + xj/2, xi/2, xj/2, yi/2, yj/2, n - 1, pts);
    hilbert_curve(x0 + xi/2 + yi/2, y0 + xj/2 + yj/2, xi/2, xj/2, yi/2, yj/2, n - 1, pts);
    hilbert_curve(x0 + xi/2 + yi, y0 + xj/2 + yj, -yi/2, -yj/2, -xi/2, -xj/2, n - 1, pts);
}

MazeHilbertPFM::MazeHilbertPFM() {
    initSettings();
}

std::vector<PFMSetting> MazeHilbertPFM::defineSettings() const {
    return {
        {"order", "Recursion Order", SettingType::Integer, 7, SettingValue(), 3, 10, 3, 10, 1},
        {"threshold", "Darkness Threshold", SettingType::Percentage, 50.0, SettingValue(), 0.0, 100.0, 0.0, 100.0, 1.0}
    };
}

std::vector<DrawingGeometry> MazeHilbertPFM::_process(const cv::Mat& image) {
    int order = m_settings["order"].toInt();
    float thresh = (m_settings["threshold"].toDouble() / 100.0f) * 255.0f;
    Path pts;
    hilbert_curve(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, order, pts);
    
    std::vector<DrawingGeometry> geoms;
    Path path;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        if (i % 1000 == 0) emitProgress(float(i) / pts.size(), geoms.size(), "Hilbert...");
        int x = std::clamp(int(pts[i].first * image.cols), 0, image.cols - 1);
        int y = std::clamp(int(pts[i].second * image.rows), 0, image.rows - 1);
        
        if ((255.0f - image.at<float>(y, x)) > thresh) {
            path.push_back({float(x), float(y)});
        } else {
            if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
            path.clear();
        }
    }
    if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    return geoms;
}


// --- maze_labyrinth_pfm.cpp ---


// --- maze_peano_pfm.cpp ---
inline Path generate_lsystem_path(const std::string& axiom, const std::map<char, std::string>& rules, int iterations, float angle_deg, float step_size) {
    std::string s = axiom;
    for (int i = 0; i < iterations; ++i) {
        std::string next_s = "";
        for (char c : s) {
            if (rules.count(c)) next_s += rules.at(c);
            else next_s += c;
        }
        s = next_s;
    }
    Path path;
    path.push_back({0.0, 0.0});
    float x = 0.0, y = 0.0, angle = 0.0;
    for (char c : s) {
        if (c == 'F') {
            x += step_size * std::cos(angle);
            y += step_size * std::sin(angle);
            path.push_back({x, y});
        } else if (c == '+') {
            angle += angle_deg * 3.141592653589793 / 180.0f;
        } else if (c == '-') {
            angle -= angle_deg * 3.141592653589793 / 180.0f;
        }
    }
    return path;
}

MazePeanoPFM::MazePeanoPFM() {
    initSettings();
}

std::vector<PFMSetting> MazePeanoPFM::defineSettings() const {
    return {
        {"order", "Recursion Order", SettingType::Integer, 4, SettingValue(), 2, 6, 2, 6, 1},
        {"threshold", "Darkness Threshold", SettingType::Percentage, 50.0, SettingValue(), 0.0, 100.0, 0.0, 100.0, 1.0}
    };
}

std::vector<DrawingGeometry> MazePeanoPFM::_process(const cv::Mat& image) {
    int order = m_settings["order"].toInt();
    float thresh = (m_settings["threshold"].toDouble() / 100.0f) * 255.0f;
    std::map<char, std::string> rules = {{'X', "XFYFX+F+YFXFY-F-XFYFX"}, {'Y', "YFXFY-F-XFYFX+F+YFXFY"}};
    emitProgress(0.2f, 0, "Generating L-System...");
    Path pts = generate_lsystem_path("X", rules, order, 90.0f, 1.0f);
    
    if (pts.empty()) return {};
    float min_x = 1e12f, max_x = -1e12f, min_y = 1e12f, max_y = -1e12f;
    for (auto& p : pts) {
        if (p.first < min_x) min_x = p.first; if (p.first > max_x) max_x = p.first;
        if (p.second < min_y) min_y = p.second; if (p.second > max_y) max_y = p.second;
    }
    
    std::vector<DrawingGeometry> geoms;
    Path path;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        float nx = (pts[i].first - min_x) / std::max(1e-5f, max_x - min_x);
        float ny = (pts[i].second - min_y) / std::max(1e-5f, max_y - min_y);
        int x = std::clamp(int(nx * image.cols), 0, image.cols - 1);
        int y = std::clamp(int(ny * image.rows), 0, image.rows - 1);
        
        if ((255.0f - image.at<float>(y, x)) > thresh) {
            path.push_back({float(x), float(y)});
        } else {
            if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
            path.clear();
        }
    }
    if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    return geoms;
}


// --- maze_rect_pfm.cpp ---


// --- maze_tri_pfm.cpp ---
std::pair<float, float> MazeTriPFM::transformPoint(float x, float y) {
    return {(x - y) * std::cos(3.141592653589793 / 6.0f), (x + y) * std::sin(3.141592653589793 / 6.0f)};
}


// --- maze_voronoi_pfm.cpp ---
std::pair<float, float> MazeVoronoiPFM::transformPoint(float x, float y) {
    return {x + std::sin(y / 10.0f) * 5.0f, y + std::cos(x / 10.0f) * 5.0f};
}


// --- mosaic_custom_pfm.cpp ---
std::vector<DrawingGeometry> MosaicCustomPFM::_process(const cv::Mat& image) {
    std::vector<DrawingGeometry> geoms = MosaicRectanglesPFM::_process(image);
    
    int cols = m_settings["cols"].toInt();
    int rows = m_settings["rows"].toInt();
    float thresh = m_settings["threshold"].toDouble();
    float cw = float(image.cols) / cols;
    float ch = float(image.rows) / rows;
    
    for (int r = 0; r < rows; ++r) {
        if (isCancelled()) break;
        emitProgress(float(r) / rows, geoms.size(), "Mosaic Custom...");
        for (int c = 0; c < cols; ++c) {
            if ((r + c) % 2 == 0) continue;
            
            cv::Rect roi(int(c * cw), int(r * ch), int(cw), int(ch));
            if (roi.x + roi.width > image.cols) roi.width = image.cols - roi.x;
            if (roi.y + roi.height > image.rows) roi.height = image.rows - roi.y;
            if (roi.width <= 0 || roi.height <= 0) continue;
            
            double mean = cv::mean(image(roi))[0];
            float dark = 255.0f - mean;
            if (dark > thresh) {
                float cx = c * cw + cw / 2.0f;
                float cy = r * ch + ch / 2.0f;
                DrawingGeometry dg;
                dg.path = generate_circle(cx, cy, std::min(cw, ch) * 0.4f, 10);
                geoms.push_back(dg);
            }
        }
    }
    return geoms;
}


// --- mosaic_rectangles_pfm.cpp ---
std::vector<DrawingGeometry> MosaicRectanglesPFM::_process(const cv::Mat& image) {
    int cols = m_settings["cols"].toInt();
    int rows = m_settings["rows"].toInt();
    float thresh = m_settings["threshold"].toDouble();
    float cw = float(image.cols) / cols;
    float ch = float(image.rows) / rows;
    
    std::vector<DrawingGeometry> geoms;
    for (int r = 0; r < rows; ++r) {
        if (isCancelled()) break;
        emitProgress(float(r) / rows, geoms.size(), "Mosaic Rectangles...");
        for (int c = 0; c < cols; ++c) {
            cv::Rect roi(int(c * cw), int(r * ch), int(cw), int(ch));
            if (roi.x + roi.width > image.cols) roi.width = image.cols - roi.x;
            if (roi.y + roi.height > image.rows) roi.height = image.rows - roi.y;
            if (roi.width <= 0 || roi.height <= 0) continue;
            
            double mean = cv::mean(image(roi))[0];
            float dark = 255.0f - mean;
            if (dark >= thresh) {
                float cx = c * cw + cw / 2.0f;
                float cy = r * ch + ch / 2.0f;
                DrawingGeometry dg;
                dg.path = generate_rectangle(cx, cy, cw * 0.9f, ch * 0.9f);
                geoms.push_back(dg);
            }
        }
    }
    return geoms;
}


// --- mosaic_voronoi_pfm.cpp ---
std::vector<PFMSetting> MosaicVoronoiPFM::defineSettings() const {
    return {
        {"cell_count", "Cell Count", SettingType::Integer, 200, SettingValue(), 20, 2000, 20, 2000, 50}
    };
}

std::vector<DrawingGeometry> MosaicVoronoiPFM::_process(const cv::Mat& image) {
    int cell_count = m_settings["cell_count"].toInt();
    std::vector<double> probs(image.cols * image.rows, 0.0);
    double sum = 0.0;
    for (int y = 0; y < image.rows; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < image.cols; ++x) {
            double d = 255.0 - row[x];
            if (d > 0) { probs[y * image.cols + x] = d; sum += d; }
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    std::vector<cv::Point2f> pts;
    for (int i = 0; i < cell_count; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        pts.push_back(cv::Point2f(idx % image.cols, idx / image.cols));
    }

    emitProgress(0.5f, 0, "Mosaic Voronoi...");
    cv::Subdiv2D subdiv(cv::Rect(0, 0, image.cols, image.rows));
    for (const auto& p : pts) subdiv.insert(p);

    std::vector<std::vector<cv::Point2f>> facets;
    std::vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);
    std::vector<DrawingGeometry> geoms;
    for (const auto& facet : facets) {
        if (facet.size() >= 2) {
            DrawingGeometry dg;
            for (const auto& pt : facet) dg.path.push_back({pt.x, pt.y});
            dg.path.push_back({facet[0].x, facet[0].y});
            geoms.push_back(dg);
        }
    }
    return geoms;
}


// --- sketch_abstract_pfm.cpp ---
SketchAbstractPFM::SketchAbstractPFM() { initSettings(); }

std::vector<PFMSetting> SketchAbstractPFM::defineSettings() const {
    return {
        {"lines", "Lines", SettingType::Integer, 500, SettingValue(), 10, 5000, 10, 5000, 10},
        {"length", "Length", SettingType::Number, 150.0, SettingValue(), 10.0, 500.0, 10.0, 500.0, 10.0}
    };
}

std::vector<DrawingGeometry> SketchAbstractPFM::_process(const cv::Mat& image) {
    int lines = m_settings["lines"].toInt();
    float length = m_settings["length"].toDouble();
    int w = image.cols;
    int h = image.rows;
    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - image.at<float>(y, x);
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;
    
    std::vector<DrawingGeometry> geoms;
    for (int i = 0; i < lines; ++i) {
        if (isCancelled()) break;
        if (i % 50 == 0) emitProgress(float(i) / lines, geoms.size(), "Sketch Abstract...");
        int idx = weightedChoice(probs);
        float cx = idx % w;
        float cy = idx / w;
        DrawingGeometry dg;
        dg.path = {{cx - randUniform(0, length), cy - randUniform(0, length)},
                   {cx + randUniform(0, length), cy + randUniform(0, length)}};
        geoms.push_back(dg);
    }
    return geoms;
}


// --- sketch_catmull_roms_pfm.cpp ---
SketchCatmullRomsPFM::SketchCatmullRomsPFM() {
    initSettings();
}

std::vector<PFMSetting> SketchCatmullRomsPFM::defineSettings() const {
    return SketchCurvesPFM().settingsList();
}

std::vector<DrawingGeometry> SketchCatmullRomsPFM::_process(const cv::Mat& image) {
    SketchCurvesPFM sketch;
    for (auto it = m_settings.begin(); it != m_settings.end(); ++it) {
        sketch.set(it->first, it->second.currentValue());
    }
    // connect removed;
    return sketch.process(image);
}


// --- sketch_cubic_beziers2_pfm.cpp ---
// -------------------------------------------------------------------------

SketchCubicBeziers2PFM::SketchCubicBeziers2PFM() {
    initSettings();
}

std::vector<PFMSetting> SketchCubicBeziers2PFM::defineSettings() const {
    return {
        {"curves", "Curves", SettingType::Integer, 1000, SettingValue(), 10, 5000, 10, 5000, 10},
        {"length", "Length", SettingType::Number, 100.0, SettingValue(), 10.0, 500.0, 10.0, 500.0, 10.0}
    };
}

std::vector<DrawingGeometry> SketchCubicBeziers2PFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int curves = m_settings["curves"].toInt();
    double length = m_settings["length"].toDouble();

    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1.0) return {};
    for (auto& p : probs) p /= sum;

    std::vector<DrawingGeometry> geoms;
    for (int i = 0; i < curves; ++i) {
        if (isCancelled()) break;
        if (i % 100 == 0) emitProgress(float(i) / curves, geoms.size(), "Generating Curves...");

        int idx = weightedChoice(probs);
        int cx = idx % w;
        int cy = idx / w;

        double p0x = cx, p0y = cy;
        double p1x = cx + randUniform(-length, length), p1y = cy + randUniform(-length, length);
        double p2x = cx + randUniform(-length, length), p2y = cy + randUniform(-length, length);
        double p3x = cx + randUniform(-length, length), p3y = cy + randUniform(-length, length);

        int steps = 20;
        Path path;
        for (int s = 0; s <= steps; ++s) {
            double t = double(s) / steps;
            double u = 1.0 - t;
            double x = u*u*u*p0x + 3*u*u*t*p1x + 3*u*t*t*p2x + t*t*t*p3x;
            double y = u*u*u*p0y + 3*u*u*t*p1y + 3*u*t*t*p2y + t*t*t*p3y;
            path.push_back({x, y});
        }
        
        DrawingGeometry dg;
        dg.path = path;
        geoms.push_back(dg);
    }

    return geoms;
}


// --- sketch_cubic_beziers_pfm.cpp ---
// ---------------------------------------------------------------------------
SketchCubicBeziersPFM::SketchCubicBeziersPFM() { initSettings(); }

std::vector<PFMSetting> SketchCubicBeziersPFM::defineSettings() const {
    auto s = makeSketchCommonSettings();
    s.push_back({ "curve_smoothness", "Curve Smoothness", SettingType::Integer, 20, {}, 1,100,1,60,1,{},"Curves" });
    return s;
}

std::vector<DrawingGeometry> SketchCubicBeziersPFM::_process(const cv::Mat& image) {
    return runSketchLoop(this, image, CurveOutputMode::CubicBezier,
                         std::max(1, get("curve_smoothness").toInt()));
}


// --- sketch_curves.cpp ---
std::vector<PFMSetting> makeSketchCommonSettings() {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,     1.0,  {}, 0.05,2.0,0.1,1.0,0.05,{},"Default" },
        { "random_seed",         "Random Seed",         SettingType::Integer,    42,   {}, 0,999999,0,999999,1,{},"Default" },
        { "should_lift_pen",     "Should Lift Pen",     SettingType::Boolean,   true,  {}, 0,1,0,1,1,{},"Style" },
        { "directionality",      "Directionality",      SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "distortion",          "Distortion",          SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "angularity",          "Angularity",          SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "edge_power",          "Edge Power",          SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "sobel_power",         "Sobel Power",         SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "luminance_power",     "Luminance Power",     SettingType::Number,   100.0,  {}, 0,200,0,200,1,{},"Style" },
        { "drawing_delta_angle", "Drawing Delta Angle", SettingType::Number,   360.0,  {}, -360,360,-360,360,1,{},"Style" },
        { "line_density",        "Line Density",        SettingType::Percentage, 75.0, {}, 0,100,0,100,1,{},"Segments" },
        { "line_min_length",     "Line Min Length",     SettingType::Number,     2.0,  {}, 1,1000,2,500,1,{},"Segments" },
        { "line_max_length",     "Line Max Length",     SettingType::Number,    40.0,  {}, 1,1000,2,500,1,{},"Segments" },
        { "line_max_limit",      "Line Max Limit",      SettingType::Integer,   -1,    {}, -1,1000000,-1,1000000,1,{},"Segments" },
        { "angle_tests",         "Angle Tests",         SettingType::Integer,    72,   {}, 1,720,1,360,1,{},"Segments" },
        { "squiggle_min_length", "Squiggle Min Length", SettingType::Number,     0.0,  {}, 0,10000,0,5000,1,{},"Squiggles" },
        { "squiggle_max_length", "Squiggle Max Length", SettingType::Number,   500.0,  {}, 0,10000,0,5000,1,{},"Squiggles" },
        { "squiggle_max_deviation","Squiggle Max Deviation",SettingType::Percentage,25.0,{},0,100,0,100,1,{},"Squiggles" },
        { "erase_min",           "Erase Min",           SettingType::Integer,    50,   {}, 0,255,0,255,1,{},"Erasing" },
        { "erase_max",           "Erase Max",           SettingType::Integer,    125,  {}, 0,255,0,255,1,{},"Erasing" },
        { "erase_radius_min",    "Erase Radius Min",    SettingType::Number,     1.0,  {}, 0,50,0,50,0.5,{},"Erasing" },
        { "erase_radius_max",    "Erase Radius Max",    SettingType::Number,     1.0,  {}, 0,50,0,50,0.5,{},"Erasing" },
        { "tone",                "Tone",                SettingType::Percentage, 50.0, {}, 0,100,0,100,1,{},"Erasing" },
    };
}

std::vector<DrawingGeometry> runSketchLoop(
    PathFindingModule* pfm,
    const cv::Mat& image,
    CurveOutputMode mode,
    int curveSmooth,
    double curveAlpha)
{
    cv::Mat workImg;
    double plotRes = pfm->get("plotting_resolution").toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image.clone();
    }
    const int W = workImg.cols, H = workImg.rows;
    cv::Mat lightened;
    workImg.convertTo(lightened, CV_32F);

    const double edgePower   = pfm->get("edge_power").toDouble();
    const double sobelPower  = pfm->get("sobel_power").toDouble();
    const double directional = pfm->get("directionality").toDouble();
    auto em = sketch_utils::buildEdgeMaps(workImg, edgePower, sobelPower, directional);

    const double lineDensity  = pfm->get("line_density").toDouble();
    const int    lineMinLen   = std::max(1, (int)pfm->get("line_min_length").toDouble());
    const int    lineMaxLen   = std::max(1, (int)pfm->get("line_max_length").toDouble());
    const int    lineMaxLimit = pfm->get("line_max_limit").toInt();
    const int    angleTests   = std::max(1, pfm->get("angle_tests").toInt());
    const double lumPower     = pfm->get("luminance_power").toDouble() / 100.0;
    const double distortion   = pfm->get("distortion").toDouble() / 100.0;
    const double angularity   = pfm->get("angularity").toDouble() / 100.0;
    const double deltaAngle   = pfm->get("drawing_delta_angle").toDouble();
    const int    sqgMin       = std::max(0, (int)pfm->get("squiggle_min_length").toDouble());
    const int    sqgMax       = std::max(1, (int)pfm->get("squiggle_max_length").toDouble());
    const double sqgDev       = pfm->get("squiggle_max_deviation").toDouble() / 100.0;
    const double eraseMin     = pfm->get("erase_min").toDouble();
    const double eraseMax     = pfm->get("erase_max").toDouble();
    const double eraseRMin    = pfm->get("erase_radius_min").toDouble();
    const double eraseRMax    = pfm->get("erase_radius_max").toDouble();
    const double tone         = pfm->get("tone").toDouble() / 100.0;

    const double deltaRad = (std::abs(deltaAngle) < 360.0)
                            ? std::abs(deltaAngle) * 3.141592653589793 / 180.0 : 2.0 * 3.141592653589793;
    const double initBright   = cv::mean(lightened)[0];
    const double targetBright = initBright + (255.0 - initBright) * (lineDensity / 100.0);
    const double sx = (double)image.cols / W, sy = (double)image.rows / H;

    const float* edgePtr  = em.edge.empty()      ? nullptr : em.edge.ptr<float>(0);
    const float* sobelPtr = em.sobel.empty()     ? nullptr : em.sobel.ptr<float>(0);
    const float* dirPtr   = em.direction.empty() ? nullptr : em.direction.ptr<float>(0);
    float* lp = lightened.ptr<float>(0);

    std::vector<double> weights(W * H);
    std::vector<DrawingGeometry> geometries;
    int totalSegments = 0, iteration = 0;

    while (iteration < W * H && !pfm->isCancelled()) {
        double dp = sketch_utils::densityProgress(lightened, initBright, targetBright);
        if (dp >= 1.0) break;
        if (lineMaxLimit > 0 && totalSegments >= lineMaxLimit) break;

        double darkSum = 0.0;
        for (int i = 0; i < W * H; ++i) {
            double d = std::max(0.0, 255.0 - (double)lp[i]);
            weights[i] = d; darkSum += d;
        }
        if (darkSum < 1.0) break;

        int startIdx = pfm->weightedChoice(weights);
        int startY = startIdx / W, startX = startIdx % W;

        // Build squiggle
        std::vector<Point> squiggle = {{ (double)startX, (double)startY }};
        double cx0 = startX, cy0 = startY;
        double startBright = lp[startY * W + startX];
        int segCount = 0;
        double prevAngle = pfm->randUniform(0.0, 2.0 * 3.141592653589793);

        for (int segI = 0; segI < sqgMax && !pfm->isCancelled(); ++segI) {
            double bestScore = -std::numeric_limits<double>::infinity();
            double bestX = cx0, bestY = cy0, bestAngle = prevAngle;
            double baseAngle = prevAngle - deltaRad / 2.0;
            double segLen = pfm->randUniform(lineMinLen, lineMaxLen);

            for (int t = 0; t < angleTests; ++t) {
                double testAngle = baseAngle + t * (deltaRad / std::max(1, angleTests));
                double angPenalty = 0.0;
                if (angularity > 0) {
                    double diff = std::abs(testAngle - prevAngle);
                    if (diff > 3.141592653589793) diff = 2.0*3.141592653589793 - diff;
                    angPenalty = diff / 3.141592653589793 * angularity;
                }
                if (distortion > 0) testAngle += pfm->randGauss(0.0, distortion * 0.5);

                double ex = std::clamp(cx0 + std::cos(testAngle)*segLen, 0.0, (double)(W-1));
                double ey = std::clamp(cy0 + std::sin(testAngle)*segLen, 0.0, (double)(H-1));
                int mxi = (int)((cx0+ex)/2), myi = (int)((cy0+ey)/2);
                int exi = (int)ex, eyi = (int)ey;

                if (mxi>=0&&mxi<W&&myi>=0&&myi<H&&exi>=0&&exi<W&&eyi>=0&&eyi<H) {
                    double score = -(lp[myi*W+mxi]+lp[eyi*W+exi])/2.0 * lumPower;
                    if (edgePtr)  score += ((double)edgePtr[myi*W+mxi] + edgePtr[eyi*W+exi])/2.0 * edgePower;
                    if (sobelPtr) score += ((double)sobelPtr[myi*W+mxi]+sobelPtr[eyi*W+exi])/2.0 * sobelPower;
                    if (dirPtr && directional > 0) {
                        double ld = dirPtr[myi*W+mxi];
                        double dd = std::abs(testAngle-ld);
                        if (dd>3.141592653589793) dd=2.0*3.141592653589793-dd;
                        score += (1.0-dd/3.141592653589793)*directional*0.5;
                    }
                    score -= angPenalty * 50.0;
                    if (score > bestScore) { bestScore=score; bestX=ex; bestY=ey; bestAngle=testAngle; }
                }
            }

            if (segI > 0 && sqgDev < 1.0) {
                int bxi=(int)bestX, byi=(int)bestY;
                if (bxi>=0&&bxi<W&&byi>=0&&byi<H) {
                    double dev = std::abs((double)lp[byi*W+bxi] - startBright) / 255.0;
                    if (dev > sqgDev && segCount >= sqgMin) break;
                }
            }

            squiggle.push_back({ bestX, bestY });
            ++segCount; ++totalSegments;
            sketch_utils::eraseLine(lightened, cx0, cy0, bestX, bestY,
                                    eraseMin, eraseMax, eraseRMin, eraseRMax, tone, W, H);
            cx0 = bestX; cy0 = bestY; prevAngle = bestAngle;
            if (segCount >= sqgMax) break;
        }

        if (squiggle.size() >= 2) {
            Path curvePts;
            if (mode == CurveOutputMode::CatmullRom) {
                curvePts = catmull_rom_chain(squiggle, curveSmooth, curveAlpha);
            } else {
                // Quad Bezier: use start, mid-control, end
                if (squiggle.size() >= 3) {
                    Point p0 = squiggle.front();
                    Point p1 = squiggle[squiggle.size() / 2];
                    Point p2 = squiggle.back();
                    curvePts = quad_bezier_path(p0, p1, p2, curveSmooth);
                } else if (mode == CurveOutputMode::CubicBezier) {
                // Cubic Bezier: use start, 1/3, 2/3, end as control points
                if (squiggle.size() >= 4) {
                    Point p0 = squiggle.front();
                    Point p1 = squiggle[squiggle.size() / 3];
                    Point p2 = squiggle[2 * squiggle.size() / 3];
                    Point p3 = squiggle.back();
                    curvePts = cubic_bezier_path(p0, p1, p2, p3, curveSmooth);
                } else if (squiggle.size() >= 3) {
                    // Fall back to quad bezier if too few points
                    Point p0 = squiggle.front();
                    Point p1 = squiggle[squiggle.size() / 2];
                    Point p2 = squiggle.back();
                    curvePts = quad_bezier_path(p0, p1, p2, curveSmooth);
                } else {
                    curvePts = squiggle;
                }
            } else {
                    curvePts = squiggle;
                }
            }
            Path scaled;
            scaled.reserve(curvePts.size());
            for (auto& p : curvePts) scaled.push_back({ p.first*sx, p.second*sy });
            geometries.push_back(DrawingGeometry{ scaled, 0 });
        }

        ++iteration;
        if (iteration % 50 == 0) {
            pfm->emitProgress((float)dp, geometries.size(),
                              "Progressing...");
        }
    }
    return geometries;
}


// --- sketch_curves_pfm.cpp ---
// ---------------------------------------------------------------------------
SketchCurvesPFM::SketchCurvesPFM() { initSettings(); }

std::vector<PFMSetting> SketchCurvesPFM::defineSettings() const {
    auto s = makeSketchCommonSettings();
    s.push_back({ "curve_smoothness", "Curve Smoothness", SettingType::Integer, 10, {}, 1,50,1,30,1,{},"Curves" });
    s.push_back({ "curve_alpha",      "Curve Alpha",      SettingType::Number,   0.5,{}, 0.0,1.0,0.0,1.0,0.1,{},"Curves" });
    return s;
}

std::vector<DrawingGeometry> SketchCurvesPFM::_process(const cv::Mat& image) {
    return runSketchLoop(this, image, CurveOutputMode::CatmullRom,
                         std::max(1, get("curve_smoothness").toInt()),
                         get("curve_alpha").toDouble());
}


// --- sketch_delaunay_pfm.cpp ---
// -------------------------------------------------------------------------
SketchDelaunayPFM::SketchDelaunayPFM() {
    initSettings();
}

std::vector<PFMSetting> SketchDelaunayPFM::defineSettings() const {
    return {
        {"nodes", "Nodes", SettingType::Integer, 500, SettingValue(), 10, 5000, 10, 5000, 10}
    };
}

std::vector<DrawingGeometry> SketchDelaunayPFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int nodes = m_settings["nodes"].toInt();

    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    std::vector<cv::Point2f> pts;
    for (int i = 0; i < nodes; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        pts.push_back(cv::Point2f(idx % w, idx / w));
    }

    cv::Subdiv2D subdiv(cv::Rect(0, 0, w, h));
    for (const auto& p : pts) subdiv.insert(p);

    std::vector<cv::Vec4f> edges;
    subdiv.getEdgeList(edges);

    std::vector<DrawingGeometry> geoms;
    for (const auto& e : edges) {
        DrawingGeometry dg;
        dg.path = {{e[0], e[1]}, {e[2], e[3]}};
        geoms.push_back(dg);
    }
    return geoms;
}


// --- sketch_flow_fields_pfm.cpp ---
// -------------------------------------------------------------------------
SketchFlowFieldsPFM::SketchFlowFieldsPFM() {
    initSettings();
}

std::vector<PFMSetting> SketchFlowFieldsPFM::defineSettings() const {
    return {
        {"lines", "Lines", SettingType::Integer, 1000, SettingValue(), 10, 10000, 10, 10000, 10}
    };
}

std::vector<DrawingGeometry> SketchFlowFieldsPFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int lines = m_settings["lines"].toInt();

    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    
    cv::GaussianBlur(gx, gx, cv::Size(5, 5), 0);
    cv::GaussianBlur(gy, gy, cv::Size(5, 5), 0);

    std::vector<DrawingGeometry> geoms;
    for (int i = 0; i < lines; ++i) {
        if (isCancelled()) break;
        if (i % 100 == 0) emitProgress(float(i) / lines, geoms.size(), "Tracing Streamlines...");

        double x = randUniform(0, w - 1);
        double y = randUniform(0, h - 1);

        Path path;
        for (int step = 0; step < 80; ++step) {
            int xi = std::clamp(int(x), 0, w - 1);
            int yi = std::clamp(int(y), 0, h - 1);
            path.push_back({x, y});

            double dx = gx.at<float>(yi, xi) * 1.5;
            double dy = gy.at<float>(yi, xi) * 1.5;

            double mag = std::hypot(dx, dy);
            if (mag < 1e-6) break;
            
            // Normalize and step
            dx = (dx / mag) * 1.5;
            dy = (dy / mag) * 1.5;

            x += dx;
            y += dy;

            if (x < 0 || x >= w || y < 0 || y >= h) break;
        }

        if (path.size() >= 2) {
            DrawingGeometry dg;
            dg.path = path;
            geoms.push_back(dg);
        }
    }

    return geoms;
}


// --- sketch_lines.cpp ---
/**
 * sketch_lines.cpp — C++ port of the SketchLines PFM algorithm.
 *
 * Algorithm (from DrawingBotV3 docs):
 *   1) Find the darkest area of the image (weighted random sample)
 *   2) Find the darkest pixel in that area
 *   3) Find the next darkest line direction from that pixel (angle tests)
 *   4) Brighten (erase) the part of the image that the line covers
 *   5) Repeat steps 2-4 until squiggle max length is reached → step 1
 *   Stop when line density or line max limit is reached.
 */

SketchLinesPFM::SketchLinesPFM() {
    initSettings();
}

std::vector<PFMSetting> SketchLinesPFM::defineSettings() const {
    return {
        // Default
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,     1.0,  {}, 0.05,-1,-1,-1, 0.05, {}, "Default","Controls the resolution of the image." },
        { "random_seed",         "Random Seed",         SettingType::Integer,    42,   {}, 0, 999999, 0, 999999, 1, {}, "Default" },
        // Style
        { "should_lift_pen",     "Should Lift Pen",     SettingType::Boolean,    true, {}, 0,1,0,1,1,{}, "Style", "Lift pen between squiggles." },
        { "directionality",      "Directionality",      SettingType::Number,     0.0,  {}, 0, 200, 0, 100, 1, {}, "Style" },
        { "distortion",          "Distortion",          SettingType::Number,     0.0,  {}, 0, 200, 0, 100, 1, {}, "Style" },
        { "angularity",          "Angularity",          SettingType::Number,     0.0,  {}, 0, 200, 0, 100, 1, {}, "Style" },
        { "edge_power",          "Edge Power",          SettingType::Number,     0.0,  {}, 0, 200, 0, 100, 1, {}, "Style" },
        { "sobel_power",         "Sobel Power",         SettingType::Number,     0.0,  {}, 0, 200, 0, 100, 1, {}, "Style" },
        { "luminance_power",     "Luminance Power",     SettingType::Number,   100.0,  {}, 0, 200, 0, 200, 1, {}, "Style" },
        { "drawing_delta_angle", "Drawing Delta Angle", SettingType::Number,   360.0,  {}, -360,360,-360,360,1,{}, "Style" },
        // Segments
        { "line_density",        "Line Density",        SettingType::Percentage, 75.0, {}, 0,100,0,100,1,  {}, "Segments" },
        { "line_min_length",     "Line Min Length",     SettingType::Number,     2.0,  {}, 1,1000,2,500,1, {}, "Segments" },
        { "line_max_length",     "Line Max Length",     SettingType::Number,     40.0, {}, 1,1000,2,500,1, {}, "Segments" },
        { "line_max_limit",      "Line Max Limit",      SettingType::Integer,    -1,   {}, -1,1000000,-1,1000000,1,{}, "Segments" },
        { "angle_tests",         "Angle Tests",         SettingType::Integer,    72,   {}, 1,720,1,360,1,  {}, "Segments" },
        // Squiggles
        { "squiggle_min_length",    "Squiggle Min Length",    SettingType::Number, 0.0,  {}, 0,10000,0,5000,1, {}, "Squiggles" },
        { "squiggle_max_length",    "Squiggle Max Length",    SettingType::Number, 500.0,{}, 0,10000,0,5000,1, {}, "Squiggles" },
        { "squiggle_max_deviation", "Squiggle Max Deviation", SettingType::Percentage, 25.0, {}, 0,100,0,100,1, {}, "Squiggles" },
        // Erasing
        { "erase_min",           "Erase Min",           SettingType::Integer,    50,   {}, 0,255,0,255,1,  {}, "Erasing" },
        { "erase_max",           "Erase Max",           SettingType::Integer,    125,  {}, 0,255,0,255,1,  {}, "Erasing" },
        { "erase_radius_min",    "Erase Radius Min",    SettingType::Number,     1.0,  {}, 0,50,0,50,0.5,  {}, "Erasing" },
        { "erase_radius_max",    "Erase Radius Max",    SettingType::Number,     1.0,  {}, 0,50,0,50,0.5,  {}, "Erasing" },
        { "tone",                "Tone",                SettingType::Percentage,  50.0, {}, 0,100,0,100,1,  {}, "Erasing" },
        // Shading
        { "shading",             "Shading",             SettingType::Boolean,    false,{}, 0,1,0,1,1,{}, "Shading" },
        { "shading_threshold",   "Shading Threshold",   SettingType::Percentage, 50.0, {}, 0,100,0,100,1,{}, "Shading" },
        { "shading_start_angle_min", "Start Angle Min", SettingType::Number, -85.0, {}, -360,360,-360,360,1,{}, "Shading" },
        { "shading_start_angle_max", "Start Angle Max", SettingType::Number,  95.0, {}, -360,360,-360,360,1,{}, "Shading" },
        { "shading_delta_angle", "Shading Delta Angle", SettingType::Number,  360.0, {}, -360,360,-360,360,1,{}, "Shading" },
    };
}

// ---------------------------------------------------------------------------
// Erase (brighten) along a line — same math as Python _erase_line
// ---------------------------------------------------------------------------
void SketchLinesPFM::eraseLine(cv::Mat& img,
                                double x0, double y0, double x1, double y1,
                                double eraseMin, double eraseMax,
                                double radiusMin, double radiusMax,
                                double tone, int w, int h) {
    double dist = std::hypot(x1 - x0, y1 - y0);
    int steps = std::max(1, (int)dist);
    float* data = img.ptr<float>(0);

    for (int s = 0; s <= steps; ++s) {
        double t = (double)s / steps;
        double toneT = (tone > 0.0) ? std::pow(t, 1.0 / std::max(0.01, tone)) : 0.5;
        double eraseVal = eraseMin + (eraseMax - eraseMin) * toneT;
        double radius   = radiusMin + (radiusMax - radiusMin) * toneT;

        double px = x0 + (x1 - x0) * t;
        double py = y0 + (y1 - y0) * t;
        int pxi = (int)px, pyi = (int)py;

        int rInt = std::max(0, (int)radius);
        if (rInt == 0) {
            if (pxi >= 0 && pxi < w && pyi >= 0 && pyi < h)
                data[pyi * w + pxi] = std::min(255.0f, data[pyi * w + pxi] + (float)eraseVal);
        } else {
            int yLo = std::max(0, pyi - rInt), yHi = std::min(h, pyi + rInt + 1);
            int xLo = std::max(0, pxi - rInt), xHi = std::min(w, pxi + rInt + 1);
            for (int ey = yLo; ey < yHi; ++ey) {
                for (int ex = xLo; ex < xHi; ++ex) {
                    double d = std::hypot(ex - px, ey - py);
                    if (d <= radius)
                        data[ey * w + ex] = std::min(255.0f, data[ey * w + ex] + (float)eraseVal);
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Main processing
// ---------------------------------------------------------------------------
std::vector<DrawingGeometry> SketchLinesPFM::_process(const cv::Mat& image) {
    // ---- apply plotting resolution ----
    cv::Mat workImg;
    double plotRes = get("plotting_resolution").toDouble();
    if (plotRes > 0.0 && std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image.clone();
    }
    const int W = workImg.cols, H = workImg.rows;

    // Working float buffer (mutable brightness)
    cv::Mat lightened;
    workImg.convertTo(lightened, CV_32F);

    // ---- Optional edge/sobel/direction maps ----
    const double edgePower   = get("edge_power").toDouble();
    const double sobelPower  = get("sobel_power").toDouble();
    const double directional = get("directionality").toDouble();

    cv::Mat edgeMap, sobelMap, directionMap;

    if (edgePower > 0) {
        cv::Mat edges;
        cv::Canny(workImg, edges, 50, 150);
        edges.convertTo(edgeMap, CV_32F, 1.0 / 255.0);
    }
    if (sobelPower > 0) {
        cv::Mat sx, sy, mag;
        cv::Sobel(workImg, sx, CV_64F, 1, 0, 3);
        cv::Sobel(workImg, sy, CV_64F, 0, 1, 3);
        cv::magnitude(sx, sy, mag);
        double mn, mx;
        cv::minMaxLoc(mag, &mn, &mx);
        mag.convertTo(sobelMap, CV_32F, 1.0 / (mx + 1e-10));
    }
    if (directional > 0) {
        cv::Mat sx, sy;
        cv::Sobel(workImg, sx, CV_64F, 1, 0, 3);
        cv::Sobel(workImg, sy, CV_64F, 0, 1, 3);
        // directionMap[y,x] = atan2(sy, sx)
        directionMap.create(H, W, CV_32F);
        for (int y = 0; y < H; ++y)
            for (int x = 0; x < W; ++x)
                directionMap.at<float>(y, x) = (float)std::atan2(
                    sy.at<double>(y, x), sx.at<double>(y, x));
    }

    // ---- Read settings ----
    const double lineDensity     = get("line_density").toDouble() / 100.0 * 100.0; // keep as %
    const int    lineMinLen      = std::max(1, (int)get("line_min_length").toDouble());
    const int    lineMaxLen      = std::max(1, (int)get("line_max_length").toDouble());
    const int    lineMaxLimit    = get("line_max_limit").toInt();
    const int    angleTests      = std::max(1, get("angle_tests").toInt());
    const double luminancePower  = get("luminance_power").toDouble() / 100.0;
    const double distortion      = get("distortion").toDouble() / 100.0;
    const double angularityVal   = get("angularity").toDouble() / 100.0;
    const double deltaAngle      = get("drawing_delta_angle").toDouble();

    const int    sqgMin          = std::max(0, (int)get("squiggle_min_length").toDouble());
    const int    sqgMax          = std::max(1, (int)get("squiggle_max_length").toDouble());
    const double sqgDeviation    = get("squiggle_max_deviation").toDouble() / 100.0;

    const double eraseMin        = get("erase_min").toDouble();
    const double eraseMax        = get("erase_max").toDouble();
    const double eraseRMin       = get("erase_radius_min").toDouble();
    const double eraseRMax       = get("erase_radius_max").toDouble();
    const double tone            = get("tone").toDouble() / 100.0;

    const bool   shadingOn       = get("shading").toBool();
    const double shadeThresh     = get("shading_threshold").toDouble() / 100.0;
    const double shadeAngleMin   = get("shading_start_angle_min").toDouble() * 3.141592653589793 / 180.0;
    const double shadeAngleMax   = get("shading_start_angle_max").toDouble() * 3.141592653589793 / 180.0;
    const double shadeDelta      = get("shading_delta_angle").toDouble() * 3.141592653589793 / 180.0;

    // ---- Scale factors to map back to original coords ----
    const double sx = (double)image.cols / W;
    const double sy = (double)image.rows / H;

    // ---- Target brightness ----
    cv::Scalar meanScalar = cv::mean(lightened);
    const double initialBrightness = meanScalar[0];
    const double targetBrightness  = initialBrightness
        + (255.0 - initialBrightness) * (lineDensity / 100.0);

    // Pre-compute delta_rad
    const double deltaRad  = (std::abs(deltaAngle) < 360.0)
                             ? (std::abs(deltaAngle) * 3.141592653589793 / 180.0)
                             : (2.0 * 3.141592653589793);

    float* lightenedPtr = lightened.ptr<float>(0);
    const float* edgePtr    = edgeMap.empty()    ? nullptr : edgeMap.ptr<float>(0);
    const float* sobelPtr   = sobelMap.empty()   ? nullptr : sobelMap.ptr<float>(0);
    const float* dirPtr     = directionMap.empty()? nullptr : directionMap.ptr<float>(0);

    std::vector<DrawingGeometry> geometries;
    int totalSegments = 0;
    int iteration     = 0;
    const int maxIterations = W * H;

    // Darkness weights vector (reused)
    std::vector<double> weights(W * H);

    while (iteration < maxIterations && !isCancelled()) {
        // ---- Check stopping conditions ----
        double curBrightness = 0.0;
        for (int i = 0; i < W * H; ++i) curBrightness += lightenedPtr[i];
        curBrightness /= (W * H);

        double densityProgress = (curBrightness - initialBrightness)
                                 / std::max(1.0, targetBrightness - initialBrightness);
        densityProgress = std::clamp(densityProgress, 0.0, 1.0);

        if (densityProgress >= 1.0) break;
        if (lineMaxLimit > 0 && totalSegments >= lineMaxLimit) break;

        bool isShading = shadingOn && (densityProgress >= shadeThresh);

        // ---- Step 1: Weighted random start position ----
        double darknessSum = 0.0;
        for (int i = 0; i < W * H; ++i) {
            double d = std::max(0.0, 255.0 - (double)lightenedPtr[i]);
            weights[i] = d;
            darknessSum += d;
        }
        if (darknessSum < 1.0) break;

        int startIdx = weightedChoice(weights);
        int startY   = startIdx / W;
        int startX   = startIdx % W;

        // ---- Step 2-4: Build a squiggle ----
        Path squiggle;
        squiggle.push_back({ (double)startX, (double)startY });
        double cx0 = (double)startX, cy0 = (double)startY;
        double startBrightness = lightenedPtr[startY * W + startX];
        int segmentCount = 0;
        double prevAngle = randUniform(0.0, 2.0 * 3.141592653589793);

        for (int segI = 0; segI < sqgMax && !isCancelled(); ++segI) {
            // Step 3: find darkest direction
            double bestScore = -std::numeric_limits<double>::infinity();
            double bestX = cx0, bestY = cy0, bestAngle = prevAngle;

            double baseAngle, testDelta;
            if (isShading) {
                baseAngle = randUniform(shadeAngleMin, shadeAngleMax);
                testDelta = shadeDelta;
            } else {
                baseAngle = prevAngle - deltaRad / 2.0;
                testDelta = deltaRad;
            }

            double segLen = randUniform(lineMinLen, lineMaxLen);

            for (int t = 0; t < angleTests; ++t) {
                double testAngle = baseAngle + t * (testDelta / std::max(1, angleTests));

                // Angularity penalty
                double angularPenalty = 0.0;
                if (angularityVal > 0 && !isShading) {
                    double diff = std::abs(testAngle - prevAngle);
                    if (diff > 3.141592653589793) diff = 2.0 * 3.141592653589793 - diff;
                    angularPenalty = diff / 3.141592653589793 * angularityVal;
                }

                // Distortion
                if (distortion > 0)
                    testAngle += randGauss(0.0, distortion * 0.5);

                double ex = cx0 + std::cos(testAngle) * segLen;
                double ey = cy0 + std::sin(testAngle) * segLen;
                ex = std::clamp(ex, 0.0, (double)(W - 1));
                ey = std::clamp(ey, 0.0, (double)(H - 1));

                double mx = (cx0 + ex) / 2.0, my = (cy0 + ey) / 2.0;
                int mxi = (int)mx, myi = (int)my;
                int exi = (int)ex, eyi = (int)ey;

                if (mxi >= 0 && mxi < W && myi >= 0 && myi < H &&
                    exi >= 0 && exi < W && eyi >= 0 && eyi < H) {
                    double bMid = lightenedPtr[myi * W + mxi];
                    double bEnd = lightenedPtr[eyi * W + exi];
                    double score = -(bMid + bEnd) / 2.0 * luminancePower;

                    if (edgePtr) {
                        double edgeScore = ((double)edgePtr[myi*W+mxi] + edgePtr[eyi*W+exi]) / 2.0;
                        score += edgeScore * edgePower;
                    }
                    if (sobelPtr) {
                        double sobelScore = ((double)sobelPtr[myi*W+mxi] + sobelPtr[eyi*W+exi]) / 2.0;
                        score += sobelScore * sobelPower;
                    }
                    if (dirPtr && directional > 0) {
                        double localDir = dirPtr[myi * W + mxi];
                        double diff = std::abs(testAngle - localDir);
                        if (diff > 3.141592653589793) diff = 2.0 * 3.141592653589793 - diff;
                        double dirScore = 1.0 - diff / 3.141592653589793;
                        score += dirScore * directional * 0.5;
                    }
                    score -= angularPenalty * 50.0;

                    if (score > bestScore) {
                        bestScore = score;
                        bestX = ex; bestY = ey;
                        bestAngle = testAngle;
                    }
                }
            }

            // Check squiggle deviation
            if (segI > 0 && sqgDeviation < 1.0) {
                int bxi = (int)bestX, byi = (int)bestY;
                if (bxi >= 0 && bxi < W && byi >= 0 && byi < H) {
                    double curB = lightenedPtr[byi * W + bxi];
                    double dev = std::abs(curB - startBrightness) / 255.0;
                    if (dev > sqgDeviation && segmentCount >= sqgMin)
                        break;
                }
            }

            squiggle.push_back({ bestX, bestY });
            ++segmentCount;
            ++totalSegments;

            eraseLine(lightened, cx0, cy0, bestX, bestY,
                      eraseMin, eraseMax, eraseRMin, eraseRMax, tone, W, H);

            cx0 = bestX; cy0 = bestY;
            prevAngle = bestAngle;
            if (segmentCount >= sqgMax) break;
        }

        // Scale back to original coords
        if (squiggle.size() >= 2) {
            Path scaled;
            scaled.reserve(squiggle.size());
            for (auto& p : squiggle)
                scaled.push_back({ p.first * sx, p.second * sy });
            geometries.push_back(DrawingGeometry{ scaled, 0 });
        }

        ++iteration;
        if (iteration % 50 == 0) {
            emitProgress(0.5f, 0, "Processing...");
        }
    }
    return geometries;
}


// --- sketch_quad_beziers2_pfm.cpp ---
// -------------------------------------------------------------------------

std::vector<DrawingGeometry> SketchQuadBeziers2PFM::_process(const cv::Mat& image) {
    return SketchCubicBeziers2PFM::_process(image);
}


// --- sketch_quad_beziers_pfm.cpp ---
// ---------------------------------------------------------------------------
SketchQuadBeziersPFM::SketchQuadBeziersPFM() { initSettings(); }

std::vector<PFMSetting> SketchQuadBeziersPFM::defineSettings() const {
    auto s = makeSketchCommonSettings();
    s.push_back({ "curve_smoothness", "Curve Smoothness", SettingType::Integer, 20, {}, 1,100,1,60,1,{},"Curves" });
    return s;
}

std::vector<DrawingGeometry> SketchQuadBeziersPFM::_process(const cv::Mat& image) {
    return runSketchLoop(this, image, CurveOutputMode::QuadBezier,
                         std::max(1, get("curve_smoothness").toInt()));
}


// --- sketch_radial_pfm.cpp ---
SketchRadialPFM::SketchRadialPFM() { initSettings(); }

std::vector<PFMSetting> SketchRadialPFM::defineSettings() const {
    return { {"lines", "Lines", SettingType::Integer, 1000, SettingValue(), 10, 5000, 10, 5000, 10} };
}

std::vector<DrawingGeometry> SketchRadialPFM::_process(const cv::Mat& image) {
    int lines = m_settings["lines"].toInt();
    int w = image.cols;
    int h = image.rows;
    float cx = w / 2.0f;
    float cy = h / 2.0f;
    std::vector<DrawingGeometry> geoms;
    for (int i = 0; i < lines; ++i) {
        if (isCancelled()) break;
        if (i % 50 == 0) emitProgress(float(i) / lines, geoms.size(), "Sketch Radial...");
        float theta = randUniform(0.0, 2.0 * 3.141592653589793);
        Path path;
        float r = 0.0f;
        while (r < std::hypot(cx, cy)) {
            float x = cx + r * std::cos(theta);
            float y = cy + r * std::sin(theta);
            if (x >= 0 && x < w && y >= 0 && y < h) {
                if (image.at<float>(int(y), int(x)) < 200.0f) {
                    path.push_back({x, y});
                } else {
                    if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
                    path.clear();
                }
            }
            r += 2.0f;
        }
        if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}


// --- sketch_scribble_pfm.cpp ---
SketchScribblePFM::SketchScribblePFM() { initSettings(); }

std::vector<PFMSetting> SketchScribblePFM::defineSettings() const {
    return {
        {"lines", "Lines", SettingType::Integer, 1000, SettingValue(), 10, 5000, 10, 5000, 10},
        {"length", "Length", SettingType::Number, 50.0, SettingValue(), 5.0, 200.0, 5.0, 200.0, 1.0}
    };
}

std::vector<DrawingGeometry> SketchScribblePFM::_process(const cv::Mat& image) {
    int lines = m_settings["lines"].toInt();
    float length = m_settings["length"].toDouble();
    int w = image.cols;
    int h = image.rows;
    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - image.at<float>(y, x);
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;
    
    std::vector<DrawingGeometry> geoms;
    for (int i = 0; i < lines; ++i) {
        if (isCancelled()) break;
        if (i % 50 == 0) emitProgress(float(i) / lines, geoms.size(), "Sketch Scribble...");
        int idx = weightedChoice(probs);
        float cx = idx % w;
        float cy = idx / w;
        Path path;
        int pts = int(randUniform(3, 8));
        for (int j = 0; j < pts; ++j) {
            path.push_back({cx + randUniform(-length, length), cy + randUniform(-length, length)});
        }
        DrawingGeometry dg; dg.path = catmull_rom_chain(path); geoms.push_back(dg);
    }
    return geoms;
}


// --- sketch_shapes.cpp ---
SketchShapesPFM::SketchShapesPFM() {
    initSettings();
}

std::vector<PFMSetting> SketchShapesPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,     1.0,  {}, 0.05,2.0,0.1,1.0,0.05 },
        { "random_seed",         "Random Seed",         SettingType::Integer,    42,   {}, 0,999999,0,999999,1 },
        { "edge_power",          "Edge Power",          SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "luminance_power",     "Luminance Power",     SettingType::Number,   100.0,  {}, 0,200,0,200,1,{},"Style" },
        { "line_density",        "Line Density",        SettingType::Percentage, 75.0, {}, 0,100,0,100,1,{},"Segments" },
        { "line_max_limit",      "Line Max Limit",      SettingType::Integer,   -1,    {}, -1,1000000,-1,1000000,1,{},"Segments" },
        { "shape_min_size",      "Shape Min Size",      SettingType::Integer,    2,    {}, 1,100,1,50,1,{},"Shapes" },
        { "shape_max_size",      "Shape Max Size",      SettingType::Integer,    40,   {}, 2,500,2,200,1,{},"Shapes" },
        { "size_tests",          "Size Tests",          SettingType::Integer,    10,   {}, 1,100,1,50,1,{},"Shapes" },
        { "enable_circles",      "Enable Circles",      SettingType::Boolean,   true,  {}, 0,1,0,1,1,{},"Shapes" },
        { "enable_squares",      "Enable Squares",      SettingType::Boolean,   true,  {}, 0,1,0,1,1,{},"Shapes" },
        { "enable_triangles",    "Enable Triangles",    SettingType::Boolean,   true,  {}, 0,1,0,1,1,{},"Shapes" },
        { "enable_stars",        "Enable Stars",        SettingType::Boolean,   true,  {}, 0,1,0,1,1,{},"Shapes" },
        { "erase_min",           "Erase Min",           SettingType::Integer,    50,   {}, 0,255,0,255,1,{},"Erasing" },
        { "erase_max",           "Erase Max",           SettingType::Integer,    125,  {}, 0,255,0,255,1,{},"Erasing" },
        { "tone",                "Tone",                SettingType::Percentage, 50.0, {}, 0,100,0,100,1,{},"Erasing" },
    };
}

std::vector<DrawingGeometry> SketchShapesPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = get("plotting_resolution").toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image.clone();
    }
    const int W = workImg.cols, H = workImg.rows;
    cv::Mat lightened;
    workImg.convertTo(lightened, CV_32F);

    auto edgeMaps = sketch_utils::buildEdgeMaps(workImg, get("edge_power").toDouble(), 0.0, 0.0);

    const double edgePower   = get("edge_power").toDouble();
    const double lumPower    = get("luminance_power").toDouble() / 100.0;
    const double lineDensity = get("line_density").toDouble();
    const int    maxLimit    = get("line_max_limit").toInt();
    const int    shMin       = std::max(1, get("shape_min_size").toInt());
    const int    shMax       = std::max(2, get("shape_max_size").toInt());
    const int    sizeTests   = std::max(1, get("size_tests").toInt());
    const double eraseMin    = get("erase_min").toDouble();
    const double eraseMax    = get("erase_max").toDouble();
    const double tone        = get("tone").toDouble() / 100.0;

    // Build enabled shape list
    std::vector<std::string> shapes;
    if (get("enable_circles").toBool())   shapes.push_back("circle");
    if (get("enable_squares").toBool())   shapes.push_back("square");
    if (get("enable_triangles").toBool()) shapes.push_back("triangle");
    if (get("enable_stars").toBool())     shapes.push_back("star");
    if (shapes.empty()) shapes.push_back("circle");

    const double initBright   = cv::mean(lightened)[0];
    const double targetBright = initBright + (255.0 - initBright) * (lineDensity / 100.0);
    const double sx = (double)image.cols / W, sy = (double)image.rows / H;

    std::vector<double> weights(W * H);
    std::vector<DrawingGeometry> geometries;
    int iteration = 0;

    while (iteration < W * H && !isCancelled()) {
        double dp = sketch_utils::densityProgress(lightened, initBright, targetBright);
        if (dp >= 1.0) break;
        if (maxLimit > 0 && (int)geometries.size() >= maxLimit) break;

        float* lp = lightened.ptr<float>(0);
        double darkSum = 0.0;
        for (int i = 0; i < W * H; ++i) {
            double d = std::max(0.0, 255.0 - (double)lp[i]);
            weights[i] = d; darkSum += d;
        }
        if (darkSum < 1.0) break;

        int idx = weightedChoice(weights);
        int cy = idx / W, cx = idx % W;

        // Pick shape type and rotation randomly
        std::string shapeType = shapes[randInt(0, shapes.size() - 1)];
        double rotation   = randUniform(0.0, 360.0);

        double bestScore = -std::numeric_limits<double>::infinity();
        double bestSize  = shMin;
        for (int t = 0; t < sizeTests; ++t) {
            double testSize = randUniform(shMin, shMax);
            double hs = testSize / 2.0;
            int x0 = std::max(0, (int)(cx - hs)), x1 = std::min(W - 1, (int)(cx + hs));
            int y0 = std::max(0, (int)(cy - hs)), y1 = std::min(H - 1, (int)(cy + hs));
            if (x1 <= x0 || y1 <= y0) continue;
            cv::Rect roi(x0, y0, x1 - x0 + 1, y1 - y0 + 1);
            double score = -cv::mean(lightened(roi))[0] * lumPower;
            if (!edgeMaps.edge.empty() && edgePower > 0)
                score += cv::mean(edgeMaps.edge(roi))[0] * edgePower * 255.0;
            if (score > bestScore) { bestScore = score; bestSize = testSize; }
        }

        double r = bestSize / 2.0;
        Path rawPath;
        if      (shapeType == "circle")   rawPath = generate_circle(cx, cy, r, std::max(12, (int)(r * 2)));
        else if (shapeType == "square")   rawPath = generate_rectangle(cx, cy, bestSize, bestSize, rotation);
        else if (shapeType == "triangle") rawPath = generate_polygon(cx, cy, r, 3, rotation);
        else if (shapeType == "star")     rawPath = generate_star(cx, cy, r, r * 0.4, 5, rotation);
        else                               rawPath = generate_circle(cx, cy, r, 12);

        Path scaledPath;
        scaledPath.reserve(rawPath.size());
        for (auto& p : rawPath) scaledPath.push_back({ p.first * sx, p.second * sy });
        geometries.push_back(DrawingGeometry{ scaledPath, 0 });

        double eraseVal = eraseMin + (eraseMax - eraseMin) * tone;
        sketch_utils::eraseRect(lightened, (int)(cx - r), (int)(cy - r),
                                (int)(cx + r), (int)(cy + r), eraseVal, W, H);

        if (++iteration % 50 == 0)
            emitProgress((float)dp, geometries.size(),
                         "Progressing...");
    }
    return geometries;
}


// --- sketch_sobel_edges_pfm.cpp ---
SketchSobelEdgesPFM::SketchSobelEdgesPFM() {
    initSettings();
}

std::vector<PFMSetting> SketchSobelEdgesPFM::defineSettings() const {
    return SketchLinesPFM().settingsList();
}

std::vector<DrawingGeometry> SketchSobelEdgesPFM::_process(const cv::Mat& image) {
    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    cv::Mat mag;
    cv::magnitude(gx, gy, mag);
    double maxVal;
    cv::minMaxLoc(mag, nullptr, &maxVal);
    cv::Mat edge_img;
    mag.convertTo(edge_img, CV_32F, 255.0 / (maxVal + 1e-6));
    edge_img = 255.0 - edge_img;

    SketchLinesPFM sketch;
    for (auto it = m_settings.begin(); it != m_settings.end(); ++it) {
        sketch.set(it->first, it->second.currentValue());
    }
    // connect removed;
    return sketch.process(edge_img);
}


// --- sketch_spirals.cpp ---
SketchSpiralsPFM::SketchSpiralsPFM() {
    initSettings();
}

std::vector<PFMSetting> SketchSpiralsPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0,   {}, 0.05, 2.0,   0.1,  1.0,  0.05 },
        { "random_seed",         "Random Seed",         SettingType::Integer, 42,    {}, 0,    999999, 0,    999999, 1 },
        { "max_radius",          "Max Radius",          SettingType::Number,  500.0, {}, 10.0, 5000.0, 10.0, 1000.0, 10.0 },
        { "density",             "Density",             SettingType::Number,  1.0,   {}, 0.1,  5.0,    0.1,  3.0,    0.1 },
    };
}

std::vector<DrawingGeometry> SketchSpiralsPFM::_process(const cv::Mat& image) {
    const int h = image.rows, w = image.cols;
    const double maxR   = get("max_radius").toDouble();
    const double density = get("density").toDouble();

    cv::Mat imgF;
    image.convertTo(imgF, CV_32F);

    const double cx = w / 2.0, cy = h / 2.0;
    Path path;
    double angle = 0.0, r = 0.0;

    while (r < maxR && !isCancelled()) {
        double x = cx + std::cos(angle) * r;
        double y = cy + std::sin(angle) * r;

        double dr;
        if (x >= 0 && x < w && y >= 0 && y < h) {
            float b = imgF.at<float>((int)y, (int)x) / 255.0f;
            path.push_back({ x, y });
            dr = (0.1 + (double)b * 2.0) * density;
        } else {
            dr = 1.0 * density;
        }
        r     += dr;
        angle += 0.1;

        if (path.size() % 1000 == 0)
            emitProgress((float)(r / maxR), 1, "" /*progress msg*/);
    }

    return { DrawingGeometry{ path, 0 } };
}


// --- sketch_squares.cpp ---
SketchSquaresPFM::SketchSquaresPFM() {
    initSettings();
}

std::vector<PFMSetting> SketchSquaresPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,     1.0,  {}, 0.05,2.0,   0.1, 1.0, 0.05 },
        { "random_seed",         "Random Seed",         SettingType::Integer,    42,   {}, 0,   999999, 0,   999999,1 },
        { "edge_power",          "Edge Power",          SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "sobel_power",         "Sobel Power",         SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "luminance_power",     "Luminance Power",     SettingType::Number,   100.0,  {}, 0,200,0,200,1,{},"Style" },
        { "line_density",        "Line Density",        SettingType::Percentage, 75.0, {}, 0,100,0,100,1,{},"Segments" },
        { "line_max_limit",      "Line Max Limit",      SettingType::Integer,   -1,    {}, -1,1000000,-1,1000000,1,{},"Segments" },
        { "square_min_size",     "Square Min Size",     SettingType::Integer,    1,    {}, 1,100,1,50,1,{},"Squares" },
        { "square_max_size",     "Square Max Size",     SettingType::Integer,    40,   {}, 2,500,2,200,1,{},"Squares" },
        { "size_tests",          "Size Tests",          SettingType::Integer,    10,   {}, 1,100,1,50,1,{},"Squares" },
        { "erase_min",           "Erase Min",           SettingType::Integer,    50,   {}, 0,255,0,255,1,{},"Erasing" },
        { "erase_max",           "Erase Max",           SettingType::Integer,    125,  {}, 0,255,0,255,1,{},"Erasing" },
        { "tone",                "Tone",                SettingType::Percentage, 50.0, {}, 0,100,0,100,1,{},"Erasing" },
    };
}

std::vector<DrawingGeometry> SketchSquaresPFM::_process(const cv::Mat& image) {
    // Apply plotting resolution
    cv::Mat workImg;
    double plotRes = get("plotting_resolution").toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image.clone();
    }
    const int W = workImg.cols, H = workImg.rows;
    cv::Mat lightened;
    workImg.convertTo(lightened, CV_32F);

    auto edgeMaps = sketch_utils::buildEdgeMaps(workImg,
        get("edge_power").toDouble(), 0.0, 0.0);

    const double edgePower   = get("edge_power").toDouble();
    const double lumPower    = get("luminance_power").toDouble() / 100.0;
    const double lineDensity = get("line_density").toDouble();
    const int    maxLimit    = get("line_max_limit").toInt();
    const int    sqMin       = std::max(1, get("square_min_size").toInt());
    const int    sqMax       = std::max(2, get("square_max_size").toInt());
    const int    sizeTests   = std::max(1, get("size_tests").toInt());
    const double eraseMin    = get("erase_min").toDouble();
    const double eraseMax    = get("erase_max").toDouble();
    const double tone        = get("tone").toDouble() / 100.0;

    const double initBright   = cv::mean(lightened)[0];
    const double targetBright = initBright + (255.0 - initBright) * (lineDensity / 100.0);
    const double sx = (double)image.cols / W, sy = (double)image.rows / H;

    std::vector<double> weights(W * H);
    std::vector<DrawingGeometry> geometries;
    int iteration = 0;

    while (iteration < W * H && !isCancelled()) {
        double dp = sketch_utils::densityProgress(lightened, initBright, targetBright);
        if (dp >= 1.0) break;
        if (maxLimit > 0 && (int)geometries.size() >= maxLimit) break;

        // Weighted random sample
        float* lp = lightened.ptr<float>(0);
        double darkSum = 0.0;
        for (int i = 0; i < W * H; ++i) {
            double d = std::max(0.0, 255.0 - (double)lp[i]);
            weights[i] = d; darkSum += d;
        }
        if (darkSum < 1.0) break;

        int idx = weightedChoice(weights);
        int cy = idx / W, cx = idx % W;

        // Test different sizes
        double bestScore = -std::numeric_limits<double>::infinity();
        double bestSize  = sqMin;

        for (int t = 0; t < sizeTests; ++t) {
            double testSize = randUniform(sqMin, sqMax);
            double hs = testSize / 2.0;
            int x0 = std::max(0, (int)(cx - hs)), x1 = std::min(W - 1, (int)(cx + hs));
            int y0 = std::max(0, (int)(cy - hs)), y1 = std::min(H - 1, (int)(cy + hs));
            if (x1 <= x0 || y1 <= y0) continue;

            // Mean brightness in region
            cv::Rect roi(x0, y0, x1 - x0 + 1, y1 - y0 + 1);
            double regionMean = cv::mean(lightened(roi))[0];
            double score = -regionMean * lumPower;

            if (!edgeMaps.edge.empty() && edgePower > 0) {
                double edgeMean = cv::mean(edgeMaps.edge(roi))[0];
                score += edgeMean * edgePower * 255.0;
            }
            if (score > bestScore) { bestScore = score; bestSize = testSize; }
        }

        // Draw square
        double hs = bestSize / 2.0;
        Path path = {
            { (cx - hs) * sx, (cy - hs) * sy },
            { (cx + hs) * sx, (cy - hs) * sy },
            { (cx + hs) * sx, (cy + hs) * sy },
            { (cx - hs) * sx, (cy + hs) * sy },
            { (cx - hs) * sx, (cy - hs) * sy }
        };
        geometries.push_back(DrawingGeometry{ path, 0 });

        // Erase
        double eraseVal = eraseMin + (eraseMax - eraseMin) * tone;
        sketch_utils::eraseRect(lightened, (int)(cx - hs), (int)(cy - hs),
                                (int)(cx + hs), (int)(cy + hs), eraseVal, W, H);

        if (++iteration % 50 == 0)
            emitProgress((float)dp, geometries.size(),
                         "Progressing...");
    }
    return geometries;
}


// --- sketch_superformula_pfm.cpp ---
// -------------------------------------------------------------------------

SketchSuperformulaPFM::SketchSuperformulaPFM() {
    initSettings();
}

std::vector<PFMSetting> SketchSuperformulaPFM::defineSettings() const {
    return {
        {"shapes", "Shapes", SettingType::Integer, 100, SettingValue(), 10, 1000, 10, 1000, 10},
        {"size", "Size", SettingType::Number, 20.0, SettingValue(), 5.0, 100.0, 5.0, 100.0, 1.0},
        {"m", "M (Symmetry)", SettingType::Number, 6.0, SettingValue(), 0.0, 20.0, 0.0, 20.0, 0.1},
        {"n1", "N1", SettingType::Number, 1.0, SettingValue(), 0.1, 10.0, 0.1, 10.0, 0.1},
        {"n2", "N2", SettingType::Number, 1.0, SettingValue(), 0.1, 10.0, 0.1, 10.0, 0.1},
        {"n3", "N3", SettingType::Number, 1.0, SettingValue(), 0.1, 10.0, 0.1, 10.0, 0.1}
    };
}

std::vector<DrawingGeometry> SketchSuperformulaPFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int shapes = m_settings["shapes"].toInt();
    double size = m_settings["size"].toDouble();
    double m = m_settings["m"].toDouble();
    double n1 = m_settings["n1"].toDouble();
    double n2 = m_settings["n2"].toDouble();
    double n3 = m_settings["n3"].toDouble();

    // Darkness weights
    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }

    if (sum < 1.0) return {};
    for (auto& p : probs) p /= sum;

    std::vector<DrawingGeometry> geoms;
    int steps = 100;
    
    for (int i = 0; i < shapes; ++i) {
        if (isCancelled()) break;
        emitProgress(float(i) / shapes, geoms.size(), "Generating Superformulas...");

        int idx = weightedChoice(probs);
        int cx = idx % w;
        int cy = idx / w;

        Path path;
        double local_size = size * (probs[idx] * sum / 255.0); // Recover approximate darkness 0-1

        for (int step = 0; step <= steps; ++step) {
            double theta = (double(step) / steps) * 2.0 * 3.141592653589793 * 2.0;

            double t1 = std::pow(std::abs(std::cos(m * theta / 4.0)), n2);
            double t2 = std::pow(std::abs(std::sin(m * theta / 4.0)), n3);
            double r = 0.0;
            if ((t1 + t2) != 0.0) {
                r = std::pow(t1 + t2, -1.0 / n1);
            }

            double x = cx + r * std::cos(theta) * local_size;
            double y = cy + r * std::sin(theta) * local_size;
            path.push_back({x, y});
        }
        
        DrawingGeometry dg;
        dg.path = path;
        geoms.push_back(dg);
    }

    return geoms;
}


// --- sketch_sweeping_curves_pfm.cpp ---
// -------------------------------------------------------------------------
SketchSweepingCurvesPFM::SketchSweepingCurvesPFM() {}

std::string SketchSweepingCurvesPFM::name() const { return "Sketch Sweeping Curves"; }


// --- sketch_voronoi_pfm.cpp ---
// -------------------------------------------------------------------------
SketchVoronoiPFM::SketchVoronoiPFM() {
    initSettings();
}

std::vector<PFMSetting> SketchVoronoiPFM::defineSettings() const {
    return {
        {"nodes", "Nodes", SettingType::Integer, 500, SettingValue(), 10, 5000, 10, 5000, 10}
    };
}

std::vector<DrawingGeometry> SketchVoronoiPFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int nodes = m_settings["nodes"].toInt();

    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    std::vector<cv::Point2f> pts;
    for (int i = 0; i < nodes; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        pts.push_back(cv::Point2f(idx % w, idx / w));
    }

    cv::Subdiv2D subdiv(cv::Rect(0, 0, w, h));
    for (const auto& p : pts) subdiv.insert(p);

    std::vector<std::vector<cv::Point2f>> facets;
    std::vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);

    std::vector<DrawingGeometry> geoms;
    for (const auto& facet : facets) {
        if (facet.size() < 2) continue;
        for (size_t i = 0; i < facet.size() - 1; ++i) {
            DrawingGeometry dg;
            dg.path = {{facet[i].x, facet[i].y}, {facet[i+1].x, facet[i+1].y}};
            geoms.push_back(dg);
        }
    }
    return geoms;
}


// --- sketch_waves.cpp ---
SketchWavesPFM::SketchWavesPFM() {
    initSettings();
}

std::vector<PFMSetting> SketchWavesPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0,  {}, 0.05, 2.0,  0.1, 1.0,  0.05, {}, "Default", "Scale factor applied to the image before processing." },
        { "random_seed",         "Random Seed",         SettingType::Integer, 42,   {}, 0,    999999,0,   999999,1,   {}, "Default" },
        { "lines_count",         "Lines Count",         SettingType::Integer, 100,  {}, 10,   1000,  10,  500,   10 },
        { "wave_freq",           "Frequency",           SettingType::Number,  10.0, {}, 1.0,  100.0, 1.0, 50.0,  1.0 },
        { "wave_amp",            "Max Amplitude",       SettingType::Number,  20.0, {}, 1.0,  100.0, 1.0, 50.0,  1.0 },
    };
}

std::vector<DrawingGeometry> SketchWavesPFM::_process(const cv::Mat& image) {
    const int h = image.rows, w = image.cols;
    const int   linesCount = get("lines_count").toInt();
    const double freq      = get("wave_freq").toDouble();
    const double maxAmp    = get("wave_amp").toDouble();

    std::vector<DrawingGeometry> geometries;
    geometries.reserve(linesCount);

    const float* imgData = reinterpret_cast<const float*>(image.data);
    // Convert once to float for fast access
    cv::Mat imgF;
    image.convertTo(imgF, CV_32F);

    for (int i = 0; i < linesCount && !isCancelled(); ++i) {
        double yBase = (double)i / std::max(1, linesCount - 1) * h;
        Path path;
        path.reserve(w);

        for (int x = 0; x < w; ++x) {
            int yi = (int)std::min((double)(h - 1), yBase);
            float brightness = imgF.at<float>(yi, x) / 255.0f;
            double amp = maxAmp * (1.0 - brightness);
            double y = yBase + std::sin((double)x / w * 3.141592653589793 * 2.0 * freq) * amp;
            path.push_back({ (double)x, y });
        }

        geometries.push_back(DrawingGeometry{ path, 0 });
        if ((i + 1) % 10 == 0)
            emitProgress((float)(i + 1) / linesCount, geometries.size(),
                         "" /*progress msg*/);
    }
    return geometries;
}


// --- spiral_circular_scribbles_pfm.cpp ---
SpiralCircularScribblesPFM::SpiralCircularScribblesPFM() {
    initSettings();
}

std::vector<PFMSetting> SpiralCircularScribblesPFM::defineSettings() const {
    return {
        {"pitch", "Pitch", SettingType::Number, 3.0, SettingValue(), 1.0, 20.0, 1.0, 20.0, 0.5},
        {"wobble", "Wobble", SettingType::Number, 2.0, SettingValue(), 0.0, 15.0, 0.0, 15.0, 0.5}
    };
}

std::vector<DrawingGeometry> SpiralCircularScribblesPFM::_process(const cv::Mat& image) {
    float pitch = m_settings["pitch"].toDouble();
    float wobble = m_settings["wobble"].toDouble();
    int w = image.cols;
    int h = image.rows;
    float cx = w / 2.0f;
    float cy = h / 2.0f;
    float max_r = std::hypot(cx, cy);
    
    Path path;
    float theta = 0.0f;
    float r = 0.0f;
    
    std::vector<DrawingGeometry> geoms;
    while (r < max_r) {
        if (isCancelled()) break;
        if (int(theta * 100) % 500 == 0) emitProgress(r / max_r, 0, "Spiral Circular Scribbles...");
        
        int xi = std::clamp(int(cx + r * std::cos(theta)), 0, w - 1);
        int yi = std::clamp(int(cy + r * std::sin(theta)), 0, h - 1);
        float dark = (255.0f - image.at<float>(yi, xi)) / 255.0f;
        
        float disp = std::sin(theta * 5.0f) * wobble * dark;
        float x = cx + (r + disp) * std::cos(theta);
        float y = cy + (r + disp) * std::sin(theta);
        
        path.push_back({x, y});
        theta += 0.12f;
        r = theta * pitch / (2.0f * 3.141592653589793);
    }
    
    if (path.size() > 1) {
        DrawingGeometry dg; dg.path = path;
        geoms.push_back(dg);
    }
    return geoms;
}


// --- spiral_sawtooth_pfm.cpp ---
SpiralSawtoothPFM::SpiralSawtoothPFM() {
    initSettings();
}

std::vector<PFMSetting> SpiralSawtoothPFM::defineSettings() const {
    return {
        {"pitch", "Pitch", SettingType::Number, 4.0, SettingValue(), 1.0, 30.0, 1.0, 30.0, 0.5},
        {"teeth", "Teeth", SettingType::Integer, 8, SettingValue(), 2, 64, 2, 64, 1}
    };
}

std::vector<DrawingGeometry> SpiralSawtoothPFM::_process(const cv::Mat& image) {
    float pitch = m_settings["pitch"].toDouble();
    int teeth = m_settings["teeth"].toInt();
    int w = image.cols;
    int h = image.rows;
    float cx = w / 2.0f;
    float cy = h / 2.0f;
    float max_r = std::hypot(cx, cy);
    
    Path path;
    float theta = 0.0f;
    float r = 0.0f;
    
    std::vector<DrawingGeometry> geoms;
    while (r < max_r) {
        if (isCancelled()) break;
        if (int(theta * 100) % 500 == 0) emitProgress(r / max_r, 0, "Spiral Sawtooth...");
        
        float tooth = std::sin(theta * teeth) * pitch * 0.3f;
        float x = cx + (r + tooth) * std::cos(theta);
        float y = cy + (r + tooth) * std::sin(theta);
        
        path.push_back({x, y});
        theta += 0.15f;
        r = theta * pitch / (2.0f * 3.141592653589793);
    }
    
    if (path.size() > 1) {
        DrawingGeometry dg; dg.path = path;
        geoms.push_back(dg);
    }
    return geoms;
}


// --- stipple_chaos_pfm.cpp ---
Path StippleChaosPFM::generateShape(float cx, float cy, float r) {
    int pts = int(randUniform(0, 4)) + 3;
    return generate_polygon(cx, cy, r, pts, randUniform(0, 360));
}


// --- stipple_circles_pfm.cpp ---
Path StippleCirclesPFM::generateShape(float cx, float cy, float r) { return generate_circle(cx, cy, r, 12); }


// --- stipple_crosses_pfm.cpp ---
Path StippleCrossesPFM::generateShape(float cx, float cy, float r) {
    return {{cx-r, cy}, {cx+r, cy}, {cx, cy}, {cx, cy-r}, {cx, cy+r}};
}


// --- stipple_dots.cpp ---
StippleDotsPFM::StippleDotsPFM() {
    initSettings();
}

std::vector<PFMSetting> StippleDotsPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0,  {}, 0.05, 2.0,   0.1,  1.0,   0.05 },
        { "random_seed",         "Random Seed",         SettingType::Integer, 42,   {}, 0,    999999, 0,    999999, 1 },
        { "num_dots",            "Number of Dots",      SettingType::Integer, 5000, {}, 100,  50000,  100,  20000,  100 },
        { "dot_size",            "Dot Radius",          SettingType::Number,  1.0,  {}, 0.1,  10.0,   0.1,  5.0,    0.1 },
    };
}

std::vector<DrawingGeometry> StippleDotsPFM::_process(const cv::Mat& image) {
    const int h = image.rows, w = image.cols;
    const int    numDots = get("num_dots").toInt();
    const double dotR    = get("dot_size").toDouble();

    cv::Mat imgF;
    image.convertTo(imgF, CV_32F);

    // Build darkness weights
    std::vector<double> weights(h * w);
    const float* fp = imgF.ptr<float>(0);
    double total = 0.0;
    for (int i = 0; i < h * w; ++i) {
        double d = std::max(0.0, 255.0 - (double)fp[i]);
        weights[i] = d;
        total += d;
    }
    if (total < 1.0) return {};

    // Place dots via weighted sampling
    std::vector<DrawingGeometry> geometries;
    geometries.reserve(numDots);

    for (int i = 0; i < numDots && !isCancelled(); ++i) {
        int idx = weightedChoice(weights);
        int cy = idx / w, cx = idx % w;

        Path circle = generate_circle((double)cx, (double)cy, dotR, 6);
        geometries.push_back(DrawingGeometry{ circle, 0 });

        if (i % 500 == 0)
            emitProgress((float)i / numDots, geometries.size(), "Stippling...");
    }
    return geometries;
}


// --- stipple_hexagons_pfm.cpp ---
Path StippleHexagonsPFM::generateShape(float cx, float cy, float r) { return generate_polygon(cx, cy, r, 6, 0); }


// --- stipple_layers_pfm.cpp ---
// ---------------------------------------------------------------------------

StippleLayersPFM::StippleLayersPFM() {
    initSettings();
}

std::vector<PFMSetting> StippleLayersPFM::defineSettings() const {
    return {
        {"plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0,  SettingValue(), 0.1, 2.0,   0.1,   2.0,   0.05},
        {"random_seed",         "Random Seed",         SettingType::Integer, 42,   SettingValue(), 0,   999999, 0,     999999, 1},
        {"layers",              "Layers",              SettingType::Integer, 4,    SettingValue(), 2,   8,      2,     8,      1},
        {"dots_per_layer",      "Dots per Layer",      SettingType::Integer, 1500, SettingValue(), 100, 20000,  100,   20000,  100},
    };
}

std::vector<DrawingGeometry> StippleLayersPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image.clone();
    }

    int n = m_settings["layers"].toInt();
    int dotsPerLayer = m_settings["dots_per_layer"].toInt();
    std::vector<DrawingGeometry> geoms;

    float step = 255.0f / n;
    for (int layer = 0; layer < n && !isCancelled(); ++layer) {
        float lo = layer * step;
        float hi = (layer + 1) * step;

        cv::Mat layerImg = cv::Mat::ones(workImg.size(), CV_8UC1) * 255;
        for (int y = 0; y < workImg.rows; ++y) {
            const uchar* src = workImg.ptr<uchar>(y);
            uchar* dst = layerImg.ptr<uchar>(y);
            for (int x = 0; x < workImg.cols; ++x) {
                float v = (float)src[x];
                if (v >= lo && v < hi) dst[x] = src[x];
            }
        }

        StippleDotsPFM stipple;
        stipple.set("num_dots", dotsPerLayer);
        stipple.set("dot_size", 1.0 + layer * 0.3);

        auto layerGeoms = stipple.process(layerImg);
        for (auto& g : layerGeoms) {
            g.penIndex = layer;
            geoms.push_back(g);
        }
        emitProgress(float(layer + 1) / n, geoms.size(),
                     "" /*progress msg*/);
    }
    return geoms;
}


// --- stipple_lines_pfm.cpp ---
Path StippleLinesPFM::generateShape(float cx, float cy, float r) {
    return {{cx-r, cy-r}, {cx+r, cy+r}};
}


// --- stipple_squares_pfm.cpp ---
Path StippleSquaresPFM::generateShape(float cx, float cy, float r) { return generate_polygon(cx, cy, r, 4, 45); }


// --- stipple_stars_pfm.cpp ---
Path StippleStarsPFM::generateShape(float cx, float cy, float r) { 
    Path p1 = generate_polygon(cx, cy, r, 5, 0);
    Path p2 = generate_polygon(cx, cy, r*0.5f, 5, 36);
    p1.insert(p1.end(), p2.begin(), p2.end());
    return p1;
}


// --- stipple_triangles_pfm.cpp ---
Path StippleTrianglesPFM::generateShape(float cx, float cy, float r) { return generate_polygon(cx, cy, r, 3, 30); }


// --- stipple_variable_circles_pfm.cpp ---
Path StippleVariableCirclesPFM::generateShape(float cx, float cy, float r) {
    return generate_circle(cx, cy, r * (0.5 + randUniform(0, 1)), 8);
}


// --- stipple_variable_squares_pfm.cpp ---
Path StippleVariableSquaresPFM::generateShape(float cx, float cy, float r) {
    return generate_polygon(cx, cy, r * (0.5 + randUniform(0, 1)), 4, randUniform(0, 90));
}


// --- streamlines_edge_field_pfm.cpp ---
std::vector<DrawingGeometry> StreamlinesEdgeFieldPFM::_process(const cv::Mat& image) {
    int count = m_settings["line_count"].toInt();
    int max_len = m_settings["max_length"].toInt();
    float step = m_settings["step_size"].toDouble();
    
    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    cv::GaussianBlur(gx, gx, cv::Size(7, 7), 0);
    cv::GaussianBlur(gy, gy, cv::Size(7, 7), 0);
    
    // For Edge field, flow is perpendicular to gradient
    cv::Mat fx = -gy;
    cv::Mat fy = gx;
    
    std::vector<DrawingGeometry> geoms;
    for (int i = 0; i < count; ++i) {
        if (isCancelled()) break;
        if (i % 20 == 0) emitProgress(float(i) / count, geoms.size(), "Edge Field Streamlines...");
        float x = randUniform(0, image.cols);
        float y = randUniform(0, image.rows);
        Path path = trace_streamline(fx, fy, x, y, max_len, step);
        if (path.size() >= 2) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}


// --- streamlines_flow_field_pfm.cpp ---
std::vector<DrawingGeometry> StreamlinesFlowFieldPFM::_process(const cv::Mat& image) {
    int count = m_settings["line_count"].toInt();
    int max_len = m_settings["max_length"].toInt();
    float step = m_settings["step_size"].toDouble();
    
    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    cv::GaussianBlur(gx, gx, cv::Size(5, 5), 0);
    cv::GaussianBlur(gy, gy, cv::Size(5, 5), 0);
    
    // For Flow field, flow is along gradient
    cv::Mat fx = gx;
    cv::Mat fy = gy;
    
    std::vector<DrawingGeometry> geoms;
    for (int i = 0; i < count; ++i) {
        if (isCancelled()) break;
        if (i % 20 == 0) emitProgress(float(i) / count, geoms.size(), "Flow Field Streamlines...");
        float x = randUniform(0, image.cols);
        float y = randUniform(0, image.rows);
        Path path = trace_streamline(fx, fy, x, y, max_len, step);
        if (path.size() >= 2) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}


// --- streamlines_superformula_pfm.cpp ---
std::vector<PFMSetting> StreamlinesSuperformulaPFM::defineSettings() const {
    auto s = BaseStreamlinePFM::defineSettings();
    s.push_back({"wave_amp", "Wave Amplitude", SettingType::Number, 2.0, SettingValue(), 0.0, 20.0, 0.0, 20.0, 0.5});
    return s;
}

std::vector<DrawingGeometry> StreamlinesSuperformulaPFM::_process(const cv::Mat& image) {
    int count = m_settings["line_count"].toInt();
    int max_len = m_settings["max_length"].toInt();
    float step = m_settings["step_size"].toDouble();
    float amp = m_settings["wave_amp"].toDouble();
    
    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    cv::GaussianBlur(gx, gx, cv::Size(3, 3), 0);
    cv::GaussianBlur(gy, gy, cv::Size(3, 3), 0);
    
    cv::Mat fx = gx;
    cv::Mat fy = gy;
    
    std::vector<DrawingGeometry> geoms;
    for (int i = 0; i < count; ++i) {
        if (isCancelled()) break;
        if (i % 20 == 0) emitProgress(float(i) / count, geoms.size(), "Superformula Streamlines...");
        float x = randUniform(0, image.cols);
        float y = randUniform(0, image.rows);
        Path path = trace_streamline(fx, fy, x, y, max_len, step);
        if (path.size() >= 2 && amp > 0) {
            Path waved;
            for (size_t j = 0; j < path.size(); ++j) {
                float wx = path[j].first + std::sin(j * 0.2f) * amp;
                float wy = path[j].second + std::cos(j * 0.15f) * amp;
                waved.push_back({wx, wy});
            }
            path = waved;
        }
        if (path.size() >= 2) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}


// --- tsp_outline_pfm.cpp ---
// -------------------------------------------------------------------------
std::vector<PFMSetting> TSPOutlinePFM::defineSettings() const {
    return {
        {"nodes", "Nodes", SettingType::Integer, 2000, SettingValue(), 100, 20000, 100, 20000, 100},
        {"edge_thresh1", "Canny Thresh 1", SettingType::Integer, 100, SettingValue(), 0, 255, 0, 255, 10},
        {"edge_thresh2", "Canny Thresh 2", SettingType::Integer, 200, SettingValue(), 0, 255, 0, 255, 10}
    };
}

std::vector<double> TSPOutlinePFM::getProbabilities(const cv::Mat& image) {
    cv::Mat u8, edges;
    image.convertTo(u8, CV_8U);
    cv::Canny(u8, edges, m_settings["edge_thresh1"].toInt(), m_settings["edge_thresh2"].toInt());

    std::vector<double> probs(image.cols * image.rows, 0.0);
    for (int y = 0; y < image.rows; ++y) {
        const uchar* row = edges.ptr<uchar>(y);
        for (int x = 0; x < image.cols; ++x) {
            if (row[x] > 0) probs[y * image.cols + x] = row[x];
        }
    }
    return probs;
}


// --- tsp_shading_pfm.cpp ---
// -------------------------------------------------------------------------
std::vector<PFMSetting> TSPShadingPFM::defineSettings() const {
    return {
        {"nodes", "Nodes", SettingType::Integer, 2000, SettingValue(), 100, 20000, 100, 20000, 100},
        {"threshold", "Darkness Cutoff", SettingType::Number, 25.0, SettingValue(), 0.0, 100.0, 0.0, 100.0, 1.0}
    };
}

std::vector<double> TSPShadingPFM::getProbabilities(const cv::Mat& image) {
    float thresh = (m_settings["threshold"].toDouble() / 100.0) * 255.0;
    std::vector<double> probs(image.cols * image.rows, 0.0);
    for (int y = 0; y < image.rows; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < image.cols; ++x) {
            float d = 255.0 - row[x];
            if (d >= thresh) probs[y * image.cols + x] = d;
        }
    }
    return probs;
}


// --- tsp_stipple_pfm.cpp ---
// -------------------------------------------------------------------------
std::vector<double> TSPStipplePFM::getProbabilities(const cv::Mat& image) {
    std::vector<double> probs(image.cols * image.rows, 0.0);
    for (int y = 0; y < image.rows; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < image.cols; ++x) {
            double d = 255.0 - row[x];
            // Only use pixels that are darker than 128 (stipple effect)
            if (d > 128.0) probs[y * image.cols + x] = d;
        }
    }
    return probs;
}


// --- tsp_voronoi_pfm.cpp ---
// -------------------------------------------------------------------------
TSPVoronoiPFM::TSPVoronoiPFM() {}

std::vector<PFMSetting> TSPVoronoiPFM::defineSettings() const {
    auto base = BaseTSPPFM::defineSettings();
    base.push_back({"voronoi_thresh", "Edge Threshold", SettingType::Integer, 80, SettingValue(), 10, 255, 10, 255, 10});
    return base;
}

std::vector<double> TSPVoronoiPFM::getProbabilities(const cv::Mat& image) {
    int thresh = m_settings.count("voronoi_thresh") ?
                 m_settings["voronoi_thresh"].toInt() : 80;
    cv::Mat edges;
    cv::Canny(image, edges, thresh, thresh * 2);
    std::vector<double> probs(image.cols * image.rows, 0.0);
    for (int y = 0; y < image.rows; ++y) {
        const uchar* row = edges.ptr<uchar>(y);
        for (int x = 0; x < image.cols; ++x) {
            if (row[x] > 0) probs[y * image.cols + x] = row[x];
        }
    }
    return probs;
}


// --- tspmstpfm.cpp ---
// -------------------------------------------------------------------------
TSPMSTPFM::TSPMSTPFM() {
    initSettings();
}

std::vector<PFMSetting> TSPMSTPFM::defineSettings() const {
    return {
        {"nodes", "Nodes", SettingType::Integer, 3000, SettingValue(), 100, 20000, 100, 20000, 100}
    };
}

std::vector<DrawingGeometry> TSPMSTPFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int nodes = m_settings["nodes"].toInt();

    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            if (d > 0) {
                probs[y * w + x] = d;
                sum += d;
            }
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    std::vector<cv::Point2f> points;
    for (int i = 0; i < nodes; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        points.push_back(cv::Point2f(idx % w, idx / w));
    }

    emitProgress(0.2f, 0, "Delaunay Triangulation...");
    cv::Subdiv2D subdiv(cv::Rect(0, 0, w, h));
    for (const auto& p : points) subdiv.insert(p);
    
    std::vector<cv::Vec4f> edgeList;
    subdiv.getEdgeList(edgeList);

    // Map exact coordinates to indices
    std::map<std::pair<float, float>, int> pt2idx;
    for (size_t i = 0; i < points.size(); ++i) {
        pt2idx[{points[i].x, points[i].y}] = i;
    }

    std::vector<std::vector<std::pair<int, float>>> adj(points.size());
    for(const auto& e : edgeList) {
        auto it1 = pt2idx.find({e[0], e[1]});
        auto it2 = pt2idx.find({e[2], e[3]});
        if (it1 != pt2idx.end() && it2 != pt2idx.end()) {
            int u = it1->second;
            int v = it2->second;
            float d = std::hypot(e[0]-e[2], e[1]-e[3]);
            adj[u].push_back({v, d});
            adj[v].push_back({u, d});
        }
    }

    emitProgress(0.6f, 0, "Solving Minimum Spanning Tree...");
    
    std::vector<bool> in_mst(points.size(), false);
    std::vector<float> min_weight(points.size(), 1e12f);
    std::vector<int> parent(points.size(), -1);
    
    std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<std::pair<float, int>>> pq;
    pq.push({0.0f, 0});
    min_weight[0] = 0.0f;
    
    std::vector<DrawingGeometry> geoms;
    
    while(!pq.empty()) {
        if (isCancelled()) return {};
        int u = pq.top().second;
        pq.pop();
        
        if (in_mst[u]) continue;
        in_mst[u] = true;
        
        if (parent[u] != -1) {
            DrawingGeometry dg;
            dg.path = {{points[parent[u]].x, points[parent[u]].y}, {points[u].x, points[u].y}};
            geoms.push_back(dg);
        }
        
        for (const auto& edge : adj[u]) {
            int v = edge.first;
            float weight = edge.second;
            if (!in_mst[v] && weight < min_weight[v]) {
                min_weight[v] = weight;
                parent[v] = u;
                pq.push({weight, v});
            }
        }
    }
    
    return geoms;
}


// --- voronoi_circles_pfm.cpp ---
std::vector<DrawingGeometry> VoronoiCirclesPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    std::vector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        if (i % 40 == 0) emitProgress(float(i) / pts.size(), geoms.size(), "Voronoi Circles...");
        float r = nearest_seed_radius(pts[i].x, pts[i].y, pts);
        DrawingGeometry dg; dg.path = generate_circle(pts[i].x, pts[i].y, r, 16);
        geoms.push_back(dg);
    }
    return geoms;
}


// --- voronoi_dashes_pfm.cpp ---
std::vector<DrawingGeometry> VoronoiDashesPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    cv::Subdiv2D subdiv(cv::Rect(0, 0, image.cols, image.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<std::vector<cv::Point2f>> facets;
    std::vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);
    std::vector<DrawingGeometry> geoms;
    for (const auto& facet : facets) {
        if (facet.size() < 2) continue;
        for (size_t i = 0; i < facet.size() - 1; ++i) {
            float x0 = facet[i].x, y0 = facet[i].y;
            float x1 = facet[i+1].x, y1 = facet[i+1].y;
            float mx = (x0 + x1) / 2.0f;
            float my = (y0 + y1) / 2.0f;
            float length = std::hypot(x1 - x0, y1 - y0) * 0.4f;
            float angle = std::atan2(y1 - y0, x1 - x0);
            DrawingGeometry dg;
            dg.path = {
                {mx - std::cos(angle) * length / 2.0f, my - std::sin(angle) * length / 2.0f},
                {mx + std::cos(angle) * length / 2.0f, my + std::sin(angle) * length / 2.0f}
            };
            geoms.push_back(dg);
        }
    }
    return geoms;
}


// --- voronoi_diagram_pfm.cpp ---
std::vector<DrawingGeometry> VoronoiDiagramPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    cv::Subdiv2D subdiv(cv::Rect(0, 0, image.cols, image.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<std::vector<cv::Point2f>> facets;
    std::vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);
    std::vector<DrawingGeometry> geoms;
    for (const auto& facet : facets) {
        if (facet.size() >= 2) {
            DrawingGeometry dg;
            for (const auto& pt : facet) dg.path.push_back({pt.x, pt.y});
            dg.path.push_back({facet[0].x, facet[0].y});
            geoms.push_back(dg);
        }
    }
    return geoms;
}


// --- voronoi_letters_pfm.cpp ---
// -------------------------------------------------------------------------
int VoronoiLettersPFM::getLloydIters() const {
    return get("lloyd_iterations").toInt();
}


// --- voronoi_shapes_pfm.cpp ---
std::vector<DrawingGeometry> VoronoiShapesPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    std::vector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        float r = nearest_seed_radius(pts[i].x, pts[i].y, pts);
        int sides = 3 + (i % 5);
        DrawingGeometry dg; dg.path = generate_polygon(pts[i].x, pts[i].y, r, sides);
        geoms.push_back(dg);
    }
    return geoms;
}


// --- voronoi_stippling_pfm.cpp ---
std::vector<DrawingGeometry> VoronoiStipplingPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    std::vector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        int xi = std::clamp(int(pts[i].x), 0, image.cols - 1);
        int yi = std::clamp(int(pts[i].y), 0, image.rows - 1);
        float dark = (255.0f - image.at<float>(yi, xi)) / 255.0f;
        float r = std::max(0.4f, nearest_seed_radius(pts[i].x, pts[i].y, pts) * 0.15f * dark);
        DrawingGeometry dg; dg.path = generate_circle(pts[i].x, pts[i].y, r, 8);
        geoms.push_back(dg);
    }
    return geoms;
}


// --- voronoi_tree_pfm.cpp ---
std::vector<DrawingGeometry> VoronoiTreePFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    cv::Subdiv2D subdiv(cv::Rect(0, 0, image.cols, image.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<cv::Vec4f> edges;
    subdiv.getEdgeList(edges);
    
    std::map<std::pair<float, float>, int> pt2idx;
    for (size_t i = 0; i < pts.size(); ++i) pt2idx[{pts[i].x, pts[i].y}] = i;
    
    std::vector<std::vector<std::pair<int, float>>> adj(pts.size());
    for (const auto& e : edges) {
        auto it1 = pt2idx.find({e[0], e[1]});
        auto it2 = pt2idx.find({e[2], e[3]});
        if (it1 != pt2idx.end() && it2 != pt2idx.end()) {
            int u = it1->second;
            int v = it2->second;
            float d = std::hypot(e[0]-e[2], e[1]-e[3]);
            adj[u].push_back({v, d});
            adj[v].push_back({u, d});
        }
    }
    
    std::vector<bool> in_mst(pts.size(), false);
    std::vector<float> min_weight(pts.size(), 1e12f);
    std::vector<int> parent(pts.size(), -1);
    std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<std::pair<float, int>>> pq;
    pq.push({0.0f, 0});
    min_weight[0] = 0.0f;
    
    std::vector<DrawingGeometry> geoms;
    while(!pq.empty()) {
        int u = pq.top().second;
        pq.pop();
        if (in_mst[u]) continue;
        in_mst[u] = true;
        if (parent[u] != -1) {
            DrawingGeometry dg; dg.path = {{pts[parent[u]].x, pts[parent[u]].y}, {pts[u].x, pts[u].y}};
            geoms.push_back(dg);
        }
        for (const auto& edge : adj[u]) {
            int v = edge.first;
            float weight = edge.second;
            if (!in_mst[v] && weight < min_weight[v]) {
                min_weight[v] = weight;
                parent[v] = u;
                pq.push({weight, v});
            }
        }
    }
    return geoms;
}


// --- voronoi_triangulation_pfm.cpp ---
std::vector<DrawingGeometry> VoronoiTriangulationPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    emitProgress(0.5, 0, "Delaunay...");
    cv::Subdiv2D subdiv(cv::Rect(0, 0, image.cols, image.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<cv::Vec4f> edges;
    subdiv.getEdgeList(edges);
    std::vector<DrawingGeometry> geoms;
    for (const auto& e : edges) {
        DrawingGeometry dg; dg.path = {{e[0], e[1]}, {e[2], e[3]}};
        geoms.push_back(dg);
    }
    return geoms;
}


// --- voronoi_tsppfm.cpp ---
std::vector<DrawingGeometry> VoronoiTSPPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    if (pts.size() < 2) return {};
    Path path = solve_tsp_nn(pts, [this](){ return isCancelled(); });
    DrawingGeometry dg; dg.path = path;
    return {dg};
}


} // end namespace pfm_ported

// ===========================================================================
// SVG Exporter
// ===========================================================================
void export_svg(const std::string& filename, const std::vector<DrawingGeometry>& geometries, int width, int height) {
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Failed to open " << filename << " for writing." << std::endl;
        return;
    }

    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" ";
    out << "width=\"" << width << "\" height=\"" << height << "\" ";
    out << "viewBox=\"0 0 " << width << " " << height << "\">\n";
    out << "  <g fill=\"none\" stroke=\"black\" stroke-width=\"1.0\">\n";

    for (const auto& geom : geometries) {
        if (geom.path.empty()) continue;
        out << "    <path d=\"M " << geom.path[0].first << " " << geom.path[0].second;
        for (std::size_t i = 1; i < geom.path.size(); ++i) {
            out << " L " << geom.path[i].first << " " << geom.path[i].second;
        }
        out << "\" />\n";
    }

    out << "  </g>\n";
    out << "</svg>\n";
    std::cout << "Saved SVG to " << filename << std::endl;
}

// ===========================================================================
// Main Application
// ===========================================================================
std::unique_ptr<PathFindingModule> create_pfm(const std::string& name) {
    if (name == "AdaptiveCircularScribblesPFM") return std::make_unique<pfm_ported::AdaptiveCircularScribblesPFM>();
    if (name == "AdaptiveDashesPFM") return std::make_unique<pfm_ported::AdaptiveDashesPFM>();
    if (name == "AdaptiveDiagramPFM") return std::make_unique<pfm_ported::AdaptiveDiagramPFM>();
    if (name == "AdaptiveLettersPFM") return std::make_unique<pfm_ported::AdaptiveLettersPFM>();
    if (name == "AdaptiveShapesPFM") return std::make_unique<pfm_ported::AdaptiveShapesPFM>();
    if (name == "AdaptiveStipplingPFM") return std::make_unique<pfm_ported::AdaptiveStipplingPFM>();
    if (name == "AdaptiveTSPPFM") return std::make_unique<pfm_ported::AdaptiveTSPPFM>();
    if (name == "AdaptiveTreePFM") return std::make_unique<pfm_ported::AdaptiveTreePFM>();
    if (name == "AdaptiveTriangulationPFM") return std::make_unique<pfm_ported::AdaptiveTriangulationPFM>();
    if (name == "AmbientFlowPFM") return std::make_unique<pfm_ported::AmbientFlowPFM>();
    if (name == "ContourPathsPFM") return std::make_unique<pfm_ported::ContourPathsPFM>();
    if (name == "EdgeShadingPFM") return std::make_unique<pfm_ported::EdgeShadingPFM>();
    if (name == "GridDashesPFM") return std::make_unique<pfm_ported::GridDashesPFM>();
    if (name == "GridLettersPFM") return std::make_unique<pfm_ported::GridLettersPFM>();
    if (name == "GridShapesPFM") return std::make_unique<pfm_ported::GridShapesPFM>();
    if (name == "Hatch3WayPFM") return std::make_unique<pfm_ported::Hatch3WayPFM>();
    if (name == "Hatch4WayPFM") return std::make_unique<pfm_ported::Hatch4WayPFM>();
    if (name == "Hatch5WayPFM") return std::make_unique<pfm_ported::Hatch5WayPFM>();
    if (name == "Hatch6WayPFM") return std::make_unique<pfm_ported::Hatch6WayPFM>();
    if (name == "HatchCircularScribblesPFM") return std::make_unique<pfm_ported::HatchCircularScribblesPFM>();
    if (name == "HatchCrossPFM") return std::make_unique<pfm_ported::HatchCrossPFM>();
    if (name == "HatchDiagonal1PFM") return std::make_unique<pfm_ported::HatchDiagonal1PFM>();
    if (name == "HatchDiagonal2PFM") return std::make_unique<pfm_ported::HatchDiagonal2PFM>();
    if (name == "HatchGridPFM") return std::make_unique<pfm_ported::HatchGridPFM>();
    if (name == "HatchHorizontalPFM") return std::make_unique<pfm_ported::HatchHorizontalPFM>();
    if (name == "HatchLinesPFM") return std::make_unique<pfm_ported::HatchLinesPFM>();
    if (name == "HatchSawtoothPFM") return std::make_unique<pfm_ported::HatchSawtoothPFM>();
    if (name == "HatchVerticalPFM") return std::make_unique<pfm_ported::HatchVerticalPFM>();
    if (name == "LBGCircularScribblesPFM") return std::make_unique<pfm_ported::LBGCircularScribblesPFM>();
    if (name == "LBGDashesPFM") return std::make_unique<pfm_ported::LBGDashesPFM>();
    if (name == "LBGDiagramPFM") return std::make_unique<pfm_ported::LBGDiagramPFM>();
    if (name == "LBGLettersPFM") return std::make_unique<pfm_ported::LBGLettersPFM>();
    if (name == "LBGShapesPFM") return std::make_unique<pfm_ported::LBGShapesPFM>();
    if (name == "LBGStipplingPFM") return std::make_unique<pfm_ported::LBGStipplingPFM>();
    if (name == "LBGTSPPFM") return std::make_unique<pfm_ported::LBGTSPPFM>();
    if (name == "LBGTreePFM") return std::make_unique<pfm_ported::LBGTreePFM>();
    if (name == "LBGTriangulationPFM") return std::make_unique<pfm_ported::LBGTriangulationPFM>();
    if (name == "LabyrinthClassicPFM") return std::make_unique<pfm_ported::LabyrinthClassicPFM>();
    if (name == "LabyrinthRomanPFM") return std::make_unique<pfm_ported::LabyrinthRomanPFM>();
    if (name == "LayersPFM") return std::make_unique<pfm_ported::LayersPFM>();
    if (name == "MazeCirclePFM") return std::make_unique<pfm_ported::MazeCirclePFM>();
    if (name == "MazeHexPFM") return std::make_unique<pfm_ported::MazeHexPFM>();
    if (name == "MazeHilbertPFM") return std::make_unique<pfm_ported::MazeHilbertPFM>();
    if (name == "MazeLabyrinthPFM") return std::make_unique<pfm_ported::MazeLabyrinthPFM>();
    if (name == "MazePeanoPFM") return std::make_unique<pfm_ported::MazePeanoPFM>();
    if (name == "MazeRectPFM") return std::make_unique<pfm_ported::MazeRectPFM>();
    if (name == "MazeTriPFM") return std::make_unique<pfm_ported::MazeTriPFM>();
    if (name == "MazeVoronoiPFM") return std::make_unique<pfm_ported::MazeVoronoiPFM>();
    if (name == "MosaicCustomPFM") return std::make_unique<pfm_ported::MosaicCustomPFM>();
    if (name == "MosaicRectanglesPFM") return std::make_unique<pfm_ported::MosaicRectanglesPFM>();
    if (name == "MosaicVoronoiPFM") return std::make_unique<pfm_ported::MosaicVoronoiPFM>();
    if (name == "SketchAbstractPFM") return std::make_unique<pfm_ported::SketchAbstractPFM>();
    if (name == "SketchCatmullRomsPFM") return std::make_unique<pfm_ported::SketchCatmullRomsPFM>();
    if (name == "SketchCubicBeziers2PFM") return std::make_unique<pfm_ported::SketchCubicBeziers2PFM>();
    if (name == "SketchCubicBeziersPFM") return std::make_unique<pfm_ported::SketchCubicBeziersPFM>();
    if (name == "SketchCurvesPFM") return std::make_unique<pfm_ported::SketchCurvesPFM>();
    if (name == "SketchDelaunayPFM") return std::make_unique<pfm_ported::SketchDelaunayPFM>();
    if (name == "SketchFlowFieldsPFM") return std::make_unique<pfm_ported::SketchFlowFieldsPFM>();
    if (name == "SketchLinesPFM") return std::make_unique<pfm_ported::SketchLinesPFM>();
    if (name == "SketchQuadBeziers2PFM") return std::make_unique<pfm_ported::SketchQuadBeziers2PFM>();
    if (name == "SketchQuadBeziersPFM") return std::make_unique<pfm_ported::SketchQuadBeziersPFM>();
    if (name == "SketchRadialPFM") return std::make_unique<pfm_ported::SketchRadialPFM>();
    if (name == "SketchScribblePFM") return std::make_unique<pfm_ported::SketchScribblePFM>();
    if (name == "SketchShapesPFM") return std::make_unique<pfm_ported::SketchShapesPFM>();
    if (name == "SketchSobelEdgesPFM") return std::make_unique<pfm_ported::SketchSobelEdgesPFM>();
    if (name == "SketchSpiralsPFM") return std::make_unique<pfm_ported::SketchSpiralsPFM>();
    if (name == "SketchSquaresPFM") return std::make_unique<pfm_ported::SketchSquaresPFM>();
    if (name == "SketchSuperformulaPFM") return std::make_unique<pfm_ported::SketchSuperformulaPFM>();
    if (name == "SketchSweepingCurvesPFM") return std::make_unique<pfm_ported::SketchSweepingCurvesPFM>();
    if (name == "SketchVoronoiPFM") return std::make_unique<pfm_ported::SketchVoronoiPFM>();
    if (name == "SketchWavesPFM") return std::make_unique<pfm_ported::SketchWavesPFM>();
    if (name == "SpiralCircularScribblesPFM") return std::make_unique<pfm_ported::SpiralCircularScribblesPFM>();
    if (name == "SpiralSawtoothPFM") return std::make_unique<pfm_ported::SpiralSawtoothPFM>();
    if (name == "StippleChaosPFM") return std::make_unique<pfm_ported::StippleChaosPFM>();
    if (name == "StippleCirclesPFM") return std::make_unique<pfm_ported::StippleCirclesPFM>();
    if (name == "StippleCrossesPFM") return std::make_unique<pfm_ported::StippleCrossesPFM>();
    if (name == "StippleDotsPFM") return std::make_unique<pfm_ported::StippleDotsPFM>();
    if (name == "StippleHexagonsPFM") return std::make_unique<pfm_ported::StippleHexagonsPFM>();
    if (name == "StippleLayersPFM") return std::make_unique<pfm_ported::StippleLayersPFM>();
    if (name == "StippleLinesPFM") return std::make_unique<pfm_ported::StippleLinesPFM>();
    if (name == "StippleSquaresPFM") return std::make_unique<pfm_ported::StippleSquaresPFM>();
    if (name == "StippleStarsPFM") return std::make_unique<pfm_ported::StippleStarsPFM>();
    if (name == "StippleTrianglesPFM") return std::make_unique<pfm_ported::StippleTrianglesPFM>();
    if (name == "StippleVariableCirclesPFM") return std::make_unique<pfm_ported::StippleVariableCirclesPFM>();
    if (name == "StippleVariableSquaresPFM") return std::make_unique<pfm_ported::StippleVariableSquaresPFM>();
    if (name == "StreamlinesEdgeFieldPFM") return std::make_unique<pfm_ported::StreamlinesEdgeFieldPFM>();
    if (name == "StreamlinesFlowFieldPFM") return std::make_unique<pfm_ported::StreamlinesFlowFieldPFM>();
    if (name == "StreamlinesSuperformulaPFM") return std::make_unique<pfm_ported::StreamlinesSuperformulaPFM>();
    if (name == "TSPClassicPFM") return std::make_unique<TSPClassicPFM>();
    if (name == "TSPMSTPFM") return std::make_unique<pfm_ported::TSPMSTPFM>();
    if (name == "TSPOutlinePFM") return std::make_unique<pfm_ported::TSPOutlinePFM>();
    if (name == "TSPShadingPFM") return std::make_unique<pfm_ported::TSPShadingPFM>();
    if (name == "TSPStipplePFM") return std::make_unique<pfm_ported::TSPStipplePFM>();
    if (name == "TSPVoronoiPFM") return std::make_unique<pfm_ported::TSPVoronoiPFM>();
    if (name == "VoronoiCirclesPFM") return std::make_unique<pfm_ported::VoronoiCirclesPFM>();
    if (name == "VoronoiDashesPFM") return std::make_unique<pfm_ported::VoronoiDashesPFM>();
    if (name == "VoronoiDiagramPFM") return std::make_unique<pfm_ported::VoronoiDiagramPFM>();
    if (name == "VoronoiLettersPFM") return std::make_unique<pfm_ported::VoronoiLettersPFM>();
    if (name == "VoronoiShapesPFM") return std::make_unique<pfm_ported::VoronoiShapesPFM>();
    if (name == "VoronoiStipplingPFM") return std::make_unique<pfm_ported::VoronoiStipplingPFM>();
    if (name == "VoronoiTSPPFM") return std::make_unique<pfm_ported::VoronoiTSPPFM>();
    if (name == "VoronoiTreePFM") return std::make_unique<pfm_ported::VoronoiTreePFM>();
    if (name == "VoronoiTriangulationPFM") return std::make_unique<pfm_ported::VoronoiTriangulationPFM>();
    return nullptr;
}

int main(int argc, char* argv[]) {
    std::cout << "imgto2d Standalone CLI\\n";

    if (argc < 4) {
        std::cerr << "Usage: imgto2d_cli <algorithm> <input_image> <output_svg> [num_nodes] [threshold]\\n";
        std::cerr << "Example: imgto2d_cli TSPClassicPFM input.jpg out.svg 2000 128\\n";
        return 1;
    }

    std::string algName = argv[1];
    std::string inputPath = argv[2];
    std::string outputPath = argv[3];

    int num_nodes = 2000;
    int threshold = 127;
    if (argc > 4) num_nodes = std::stoi(argv[4]);
    if (argc > 5) threshold = std::stoi(argv[5]);

    auto pfm = create_pfm(algName);
    if (!pfm) {
        std::cerr << "Error: Unknown algorithm '" << algName << "'\\n";
        return 1;
    }

    cv::Mat img = cv::imread(inputPath, cv::IMREAD_COLOR);
    if (img.empty()) {
        std::cerr << "Error: Could not load image from " << inputPath << "\\n";
        return 1;
    }
    std::cout << "Processing: " << inputPath << " (" << img.cols << "x" << img.rows << ") with " << algName << "\\n";

    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    ThresholdFilter tFilter;
    tFilter.set("threshold", threshold);
    cv::Mat filtered = tFilter.process(gray);

    pfm->set("nodes", num_nodes);
    pfm->set("num_lines", num_nodes);
    pfm->set("lines", num_nodes);
    
    auto geometries = pfm->process(filtered);
    export_svg(outputPath, geometries, img.cols, img.rows);
    std::cout << "Saved SVG to " << outputPath << "\\n";
    return 0;
}