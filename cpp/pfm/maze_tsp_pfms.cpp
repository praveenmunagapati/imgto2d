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
        const float* row = image.ptr<float>(y);
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
        const float* row = image.ptr<float>(y);
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
        const float* row = image.ptr<float>(y);
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
