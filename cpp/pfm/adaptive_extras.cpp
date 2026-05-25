#include "pfm/adaptive_extras.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <cmath>
#include <map>
#include <queue>

// ---------------------------------------------------------------------------
// Shared helpers
// ---------------------------------------------------------------------------

static Path solve_tsp_adaptive(const std::vector<cv::Point2f>& points,
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
// BaseAdaptivePFM
// ---------------------------------------------------------------------------

BaseAdaptivePFM::BaseAdaptivePFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> BaseAdaptivePFM::defineSettings() const {
    return {
        {"plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0, QVariant(), 0.1, 2.0,   0.1, 2.0,   0.05},
        {"random_seed",         "Random Seed",         SettingType::Integer, 42,  QVariant(), 0,   999999, 0,   999999, 1},
        {"cell_count",          "Cell Count",          SettingType::Integer, 800, QVariant(), 50,  20000,  50,  20000,  50},
        {"lloyd_iterations",    "Lloyd Iterations",    SettingType::Integer, 3,   QVariant(), 0,   20,     0,   10,     1},
        {"min_brightness",      "Min Brightness (%)",  SettingType::Number,  80.0, QVariant(), 0.0, 100.0, 0.0, 100.0, 1.0},
    };
}

std::vector<cv::Point2f> BaseAdaptivePFM::getSeeds(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int cellCount   = m_settings["cell_count"].toInt();
    int lloydIters  = m_settings["lloyd_iterations"].toInt();
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

// ---------------------------------------------------------------------------
// AdaptiveCircularScribblesPFM
// ---------------------------------------------------------------------------

AdaptiveCircularScribblesPFM::AdaptiveCircularScribblesPFM(QObject* p)
    : BaseAdaptivePFM(p) {}

QVector<PFMSetting> AdaptiveCircularScribblesPFM::defineSettings() const {
    auto s = BaseAdaptivePFM::defineSettings();
    s.append({"scribble_turns", "Scribble Turns", SettingType::Number, 2.0, QVariant(), 0.5, 8.0, 0.5, 8.0, 0.5});
    s.append({"radius_scale",   "Radius Scale",   SettingType::Number, 3.0, QVariant(), 0.5, 20.0, 0.5, 20.0, 0.5});
    return s;
}

QVector<DrawingGeometry> AdaptiveCircularScribblesPFM::_process(const cv::Mat& image) {
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
    QVector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        if (i % 50 == 0) emitProgress(float(i) / pts.size(), (int)geoms.size(), "Adaptive scribbles...");
        float cx = pts[i].x, cy = pts[i].y;
        int steps = std::max(12, (int)(turns * 24));
        Path path;
        for (int s = 0; s <= steps; ++s) {
            double t = (double)s / steps * turns * M_PI * 2.0;
            path.push_back({cx + std::cos(t) * (float)rscale,
                            cy + std::sin(t) * (float)rscale});
        }
        geoms.push_back(DrawingGeometry{path, 0});
    }
    return geoms;
}

// ---------------------------------------------------------------------------
// AdaptiveShapesPFM
// ---------------------------------------------------------------------------

AdaptiveShapesPFM::AdaptiveShapesPFM(QObject* p) : BaseAdaptivePFM(p) {}

QVector<PFMSetting> AdaptiveShapesPFM::defineSettings() const {
    auto s = BaseAdaptivePFM::defineSettings();
    s.append({"shape_size", "Shape Size", SettingType::Number, 4.0, QVariant(), 1.0, 30.0, 1.0, 30.0, 0.5});
    return s;
}

QVector<DrawingGeometry> AdaptiveShapesPFM::_process(const cv::Mat& image) {
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
    QVector<DrawingGeometry> geoms;
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

// ---------------------------------------------------------------------------
// AdaptiveDashesPFM
// ---------------------------------------------------------------------------

QVector<DrawingGeometry> AdaptiveDashesPFM::_process(const cv::Mat& image) {
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
    QVector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        float cx = pts[i].x, cy = pts[i].y;
        float angle  = (float)randUniform(0.0, M_PI);
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

// ---------------------------------------------------------------------------
// AdaptiveTSPPFM
// ---------------------------------------------------------------------------

QVector<DrawingGeometry> AdaptiveTSPPFM::_process(const cv::Mat& image) {
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

// ---------------------------------------------------------------------------
// AdaptiveTriangulationPFM
// ---------------------------------------------------------------------------

QVector<DrawingGeometry> AdaptiveTriangulationPFM::_process(const cv::Mat& image) {
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
    QVector<DrawingGeometry> geoms;
    for (const auto& e : edges) {
        geoms.push_back(DrawingGeometry{{{e[0], e[1]}, {e[2], e[3]}}, 0});
    }
    emitProgress(1.0f, (int)geoms.size(), "Triangulation done");
    return geoms;
}

// ---------------------------------------------------------------------------
// AdaptiveTreePFM (Minimum Spanning Tree)
// ---------------------------------------------------------------------------

QVector<DrawingGeometry> AdaptiveTreePFM::_process(const cv::Mat& image) {
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
    QVector<DrawingGeometry> geoms;
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

// ---------------------------------------------------------------------------
// AdaptiveDiagramPFM (Voronoi cells)
// ---------------------------------------------------------------------------

QVector<DrawingGeometry> AdaptiveDiagramPFM::_process(const cv::Mat& image) {
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
    QVector<DrawingGeometry> geoms;
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
