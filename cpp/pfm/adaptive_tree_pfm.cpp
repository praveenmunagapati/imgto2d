#include "pfm/adaptive_tree_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <cmath>
#include <map>
#include <queue>

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
