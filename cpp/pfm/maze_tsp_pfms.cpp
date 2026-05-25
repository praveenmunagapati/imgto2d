#include "pfm/maze_tsp_pfms.h"
#include <opencv2/imgproc.hpp>
#include <map>
#include <queue>
#include <cmath>

static Path solve_tsp_nn(const std::vector<cv::Point2f>& points, std::function<bool()> isCancelledFunc, std::function<void(float)> progressFunc) {
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
        if (remaining % 1000 == 0) progressFunc(1.0f - (float(remaining) / total));

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

// -------------------------------------------------------------------------
// BaseTSPPFM
// -------------------------------------------------------------------------
BaseTSPPFM::BaseTSPPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}
QVector<PFMSetting> BaseTSPPFM::defineSettings() const {
    return {
        {"nodes", "Nodes", SettingType::Integer, 2000, QVariant(), 100, 20000, 100, 20000, 100}
    };
}
QVector<DrawingGeometry> BaseTSPPFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int nodes = m_settings["nodes"].toInt();

    std::vector<double> probs = getProbabilities(image);
    if (probs.empty()) return {};
    
    double sum = 0.0;
    for (auto p : probs) sum += p;
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    std::vector<cv::Point2f> points;
    for (int i = 0; i < nodes; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        points.push_back(cv::Point2f(idx % w, idx / w));
    }

    auto path = solve_tsp_nn(points, [this](){ return isCancelled(); }, [this](float p){ emitProgress(0.5f + p * 0.5f, 0, "Solving TSP..."); });
    if (path.size() >= 2) {
        DrawingGeometry dg;
        dg.path = path;
        return {dg};
    }
    return {};
}

// -------------------------------------------------------------------------
// TSPClassicPFM
// -------------------------------------------------------------------------
std::vector<double> TSPClassicPFM::getProbabilities(const cv::Mat& image) {
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

// -------------------------------------------------------------------------
// TSPOutlinePFM
// -------------------------------------------------------------------------
QVector<PFMSetting> TSPOutlinePFM::defineSettings() const {
    return {
        {"nodes", "Nodes", SettingType::Integer, 2000, QVariant(), 100, 20000, 100, 20000, 100},
        {"edge_thresh1", "Canny Thresh 1", SettingType::Integer, 100, QVariant(), 0, 255, 0, 255, 10},
        {"edge_thresh2", "Canny Thresh 2", SettingType::Integer, 200, QVariant(), 0, 255, 0, 255, 10}
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

// -------------------------------------------------------------------------
// TSPShadingPFM
// -------------------------------------------------------------------------
QVector<PFMSetting> TSPShadingPFM::defineSettings() const {
    return {
        {"nodes", "Nodes", SettingType::Integer, 2000, QVariant(), 100, 20000, 100, 20000, 100},
        {"threshold", "Darkness Cutoff", SettingType::Number, 25.0, QVariant(), 0.0, 100.0, 0.0, 100.0, 1.0}
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

// -------------------------------------------------------------------------
// TSPMSTPFM
// -------------------------------------------------------------------------
TSPMSTPFM::TSPMSTPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}
QVector<PFMSetting> TSPMSTPFM::defineSettings() const {
    return {
        {"nodes", "Nodes", SettingType::Integer, 3000, QVariant(), 100, 20000, 100, 20000, 100}
    };
}
QVector<DrawingGeometry> TSPMSTPFM::_process(const cv::Mat& image) {
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
    
    QVector<DrawingGeometry> geoms;
    
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

// ---------------------------------------------------------------------------
// Phase 7: Mazes and Labyrinths
// ---------------------------------------------------------------------------

static Path generate_dfs_maze(int width, int height, BaseMazePFM* pfm) {
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

BaseMazePFM::BaseMazePFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> BaseMazePFM::defineSettings() const {
    return {
        {"complexity", "Complexity", SettingType::Integer, 10, QVariant(), 1, 50, 1, 50, 1}
    };
}

std::pair<float, float> BaseMazePFM::transformPoint(float x, float y) { return {x, y}; }
std::pair<float, float> MazeHexPFM::transformPoint(float x, float y) {
    float offset = (int(y / 10.0) % 2 != 0) ? 5.0f : 0.0f;
    return {x + offset, y * std::sqrt(3.0f) / 2.0f};
}
std::pair<float, float> MazeTriPFM::transformPoint(float x, float y) {
    return {(x - y) * std::cos(M_PI / 6.0f), (x + y) * std::sin(M_PI / 6.0f)};
}
std::pair<float, float> MazeVoronoiPFM::transformPoint(float x, float y) {
    return {x + std::sin(y / 10.0f) * 5.0f, y + std::cos(x / 10.0f) * 5.0f};
}
std::pair<float, float> MazeCirclePFM::transformPoint(float x, float y) {
    float r = y + 10.0f;
    float theta = x / 10.0f;
    return {r * std::cos(theta), r * std::sin(theta)};
}

QVector<DrawingGeometry> BaseMazePFM::_process(const cv::Mat& image) {
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

LabyrinthClassicPFM::LabyrinthClassicPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> LabyrinthClassicPFM::defineSettings() const {
    return {
        {"spacing", "Spiral Spacing", SettingType::Number, 10.0, QVariant(), 2.0, 50.0, 2.0, 50.0, 1.0},
        {"wobble", "Wobble Factor", SettingType::Number, 0.0, QVariant(), 0.0, 10.0, 0.0, 10.0, 0.5}
    };
}

QVector<DrawingGeometry> LabyrinthClassicPFM::_process(const cv::Mat& image) {
    float spacing = m_settings["spacing"].toDouble();
    float wobble = m_settings["wobble"].toDouble();
    float cx = image.cols / 2.0f;
    float cy = image.rows / 2.0f;
    float max_r = std::hypot(cx, cy);
    float theta = 0.0f;
    float b = spacing / (2.0f * M_PI);
    
    QVector<DrawingGeometry> geoms;
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

static void hilbert_curve(float x0, float y0, float xi, float xj, float yi, float yj, int n, Path& pts) {
    if (n <= 0) { pts.push_back({x0 + (xi + yi) / 2.0f, y0 + (xj + yj) / 2.0f}); return; }
    hilbert_curve(x0, y0, yi/2, yj/2, xi/2, xj/2, n - 1, pts);
    hilbert_curve(x0 + xi/2, y0 + xj/2, xi/2, xj/2, yi/2, yj/2, n - 1, pts);
    hilbert_curve(x0 + xi/2 + yi/2, y0 + xj/2 + yj/2, xi/2, xj/2, yi/2, yj/2, n - 1, pts);
    hilbert_curve(x0 + xi/2 + yi, y0 + xj/2 + yj, -yi/2, -yj/2, -xi/2, -xj/2, n - 1, pts);
}

MazeHilbertPFM::MazeHilbertPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> MazeHilbertPFM::defineSettings() const {
    return {
        {"order", "Recursion Order", SettingType::Integer, 7, QVariant(), 3, 10, 3, 10, 1},
        {"threshold", "Darkness Threshold", SettingType::Percentage, 50.0, QVariant(), 0.0, 100.0, 0.0, 100.0, 1.0}
    };
}

QVector<DrawingGeometry> MazeHilbertPFM::_process(const cv::Mat& image) {
    int order = m_settings["order"].toInt();
    float thresh = (m_settings["threshold"].toDouble() / 100.0f) * 255.0f;
    Path pts;
    hilbert_curve(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, order, pts);
    
    QVector<DrawingGeometry> geoms;
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

static Path generate_lsystem_path(const std::string& axiom, const std::map<char, std::string>& rules, int iterations, float angle_deg, float step_size) {
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
            angle += angle_deg * M_PI / 180.0f;
        } else if (c == '-') {
            angle -= angle_deg * M_PI / 180.0f;
        }
    }
    return path;
}

MazePeanoPFM::MazePeanoPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> MazePeanoPFM::defineSettings() const {
    return {
        {"order", "Recursion Order", SettingType::Integer, 4, QVariant(), 2, 6, 2, 6, 1},
        {"threshold", "Darkness Threshold", SettingType::Percentage, 50.0, QVariant(), 0.0, 100.0, 0.0, 100.0, 1.0}
    };
}

QVector<DrawingGeometry> MazePeanoPFM::_process(const cv::Mat& image) {
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
    
    QVector<DrawingGeometry> geoms;
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
