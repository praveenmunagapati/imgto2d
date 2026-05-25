#include "pfm/voronoi_pfms.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <map>
#include <queue>
#include <cmath>

static float nearest_seed_radius(float cx, float cy, const std::vector<cv::Point2f>& points) {
    float best = 1e12f;
    for (const auto& pt : points) {
        float dx = pt.x - cx;
        float dy = pt.y - cy;
        if (std::abs(dx) < 1e-6f && std::abs(dy) < 1e-6f) continue;
        float d = std::hypot(dx, dy);
        if (d < best) best = d;
    }
    return best < 1e11f ? best * 0.45f : 8.0f;
}

static Path solve_tsp_nn(const std::vector<cv::Point2f>& points, std::function<bool()> isCancelledFunc) {
    if (points.empty()) return {};
    std::vector<bool> visited(points.size(), false);
    Path path;
    path.reserve(points.size());
    int current = 0;
    visited[0] = true;
    path.push_back({points[0].x, points[0].y});
    int remaining = points.size() - 1;

    while (remaining > 0) {
        if (isCancelledFunc()) return {};
        int best_i = -1;
        float best_d = 1e12f;
        float cx = points[current].x;
        float cy = points[current].y;
        for (size_t i = 0; i < points.size(); ++i) {
            if (!visited[i]) {
                float dx = points[i].x - cx;
                float dy = points[i].y - cy;
                float d = dx*dx + dy*dy;
                if (d < best_d) { best_d = d; best_i = i; }
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

BaseVoronoiExtraPFM::BaseVoronoiExtraPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> BaseVoronoiExtraPFM::defineSettings() const {
    return {
        {"cell_count", "Cell Count", SettingType::Integer, 800, QVariant(), 50, 20000, 50, 20000, 100},
        {"lloyd_iterations", "Lloyd Iterations", SettingType::Integer, 5, QVariant(), 0, 20, 0, 20, 1},
        {"min_brightness", "Min Brightness", SettingType::Number, 0.0, QVariant(), 0.0, 255.0, 0.0, 255.0, 1.0}
    };
}

std::vector<cv::Point2f> BaseVoronoiExtraPFM::getSeeds(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int cell_count = m_settings["cell_count"].toInt();
    int lloyd_iters = m_settings["lloyd_iterations"].toInt();
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

QVector<DrawingGeometry> VoronoiCirclesPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    QVector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        if (i % 40 == 0) emitProgress(float(i) / pts.size(), geoms.size(), "Voronoi Circles...");
        float r = nearest_seed_radius(pts[i].x, pts[i].y, pts);
        DrawingGeometry dg; dg.path = generate_circle(pts[i].x, pts[i].y, r, 16);
        geoms.push_back(dg);
    }
    return geoms;
}

QVector<DrawingGeometry> VoronoiTriangulationPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    emitProgress(0.5, 0, "Delaunay...");
    cv::Subdiv2D subdiv(cv::Rect(0, 0, image.cols, image.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<cv::Vec4f> edges;
    subdiv.getEdgeList(edges);
    QVector<DrawingGeometry> geoms;
    for (const auto& e : edges) {
        DrawingGeometry dg; dg.path = {{e[0], e[1]}, {e[2], e[3]}};
        geoms.push_back(dg);
    }
    return geoms;
}

QVector<DrawingGeometry> VoronoiStipplingPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    QVector<DrawingGeometry> geoms;
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

QVector<DrawingGeometry> VoronoiDashesPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    cv::Subdiv2D subdiv(cv::Rect(0, 0, image.cols, image.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<std::vector<cv::Point2f>> facets;
    std::vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);
    QVector<DrawingGeometry> geoms;
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

QVector<DrawingGeometry> VoronoiDiagramPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    cv::Subdiv2D subdiv(cv::Rect(0, 0, image.cols, image.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<std::vector<cv::Point2f>> facets;
    std::vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);
    QVector<DrawingGeometry> geoms;
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

QVector<DrawingGeometry> VoronoiShapesPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    QVector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        float r = nearest_seed_radius(pts[i].x, pts[i].y, pts);
        int sides = 3 + (i % 5);
        DrawingGeometry dg; dg.path = generate_polygon(pts[i].x, pts[i].y, r, sides);
        geoms.push_back(dg);
    }
    return geoms;
}

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

QVector<DrawingGeometry> VoronoiTSPPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    if (pts.size() < 2) return {};
    Path path = solve_tsp_nn(pts, [this](){ return isCancelled(); });
    DrawingGeometry dg; dg.path = path;
    return {dg};
}
