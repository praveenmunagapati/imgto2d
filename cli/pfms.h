#pragma once
#include "core_types.h"
#include <memory>

namespace pfm_ported {


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

    std::discrete_distribution<int> m_cachedDist;
    const std::vector<double>* m_cachedWeightsPtr = nullptr;
    size_t m_cachedWeightsSize = 0;
    double m_cachedFirstWeight = -1.0;
    double m_cachedLastWeight = -1.0;

    int weightedChoice(const std::vector<double>& weights) {
        if (weights.empty()) return 0;
        if (&weights != m_cachedWeightsPtr || weights.size() != m_cachedWeightsSize || 
            weights.front() != m_cachedFirstWeight || weights.back() != m_cachedLastWeight) {
            m_cachedDist = std::discrete_distribution<int>(weights.begin(), weights.end());
            m_cachedWeightsPtr = &weights;
            m_cachedWeightsSize = weights.size();
            m_cachedFirstWeight = weights.front();
            m_cachedLastWeight = weights.back();
        }
        return m_cachedDist(m_rng);
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
        
        #pragma omp parallel
        {
            int local_best_i = -1;
            float local_best_d = 1e12f;
            
            #pragma omp for nowait
            for (int i = 0; i < (int)points.size(); ++i) {
                if (!visited[i]) {
                    float dx = points[i].x - cx;
                    float dy = points[i].y - cy;
                    float d = dx*dx + dy*dy;
                    if (d < local_best_d) {
                        local_best_d = d;
                        local_best_i = i;
                    }
                }
            }
            
            #pragma omp critical
            {
                if (local_best_d < best_d) {
                    best_d = local_best_d;
                    best_i = local_best_i;
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
        #pragma omp parallel for

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

// ===========================================================================


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
    explicit AdaptiveDashesPFM() { initSettings(); }
    std::string name() const override { return "Adaptive Dashes"; }
protected:
    std::vector<PFMSetting>      defineSettings() const override;
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
class HatchCircularScribblesPFM : public BaseMultiHatchPFM {
public:
    explicit HatchCircularScribblesPFM();
    std::string name() const override { return "Hatch Circular Scribbles"; }
    bool isPremium() const override { return true; }
protected:
    std::vector<float> getAngles() const override { return { (float)get("angle").toDouble() }; }
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
class HatchSawtoothPFM : public BaseMultiHatchPFM {
public:
    explicit HatchSawtoothPFM();
    std::string name() const override { return "Hatch Sawtooth"; }
    bool isPremium() const override { return true; }
protected:
    std::vector<float> getAngles() const override { return { (float)get("angle").toDouble() }; }
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
public: MosaicVoronoiPFM(); std::string name() const override{return "Mosaic Voronoi";} protected: std::vector<PFMSetting> defineSettings() const override; std::vector<DrawingGeometry> _process(const cv::Mat& image) override; };


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
public:
    explicit VoronoiShapesPFM() { initSettings(); }
    std::string name() const override { return "Voronoi Shapes"; }
protected:
    std::vector<PFMSetting>      defineSettings() const override;
    std::vector<DrawingGeometry> _process(const cv::Mat& image) override;
};


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










std::unique_ptr<PathFindingModule> create_pfm(const std::string& name);

} // namespace pfm_ported
