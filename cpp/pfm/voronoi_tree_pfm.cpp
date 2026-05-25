#include "pfm/voronoi_tree_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <map>
#include <queue>
#include <cmath>

QVector<DrawingGeometry> VoronoiTreePFM::_process(const cv::Mat& image) {
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
    
    QVector<DrawingGeometry> geoms;
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
