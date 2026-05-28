#include "../include/PFM_Adaptive.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/flann.hpp>
#include <cmath>

namespace DrawingBot {
    
    static std::vector<cv::Point2f> generateAdaptivePoints(const cv::Mat& ref, const AdaptiveBaseSettings& settings) {
        cv::Mat gray;
        if (ref.channels() == 3) cv::cvtColor(ref, gray, cv::COLOR_BGR2GRAY); else gray = ref.clone();
        
        // Adjust brightness/contrast
        gray.convertTo(gray, -1, settings.contrast, (settings.brightness - 1.0f) * 128.0f);
        
        std::vector<cv::Point2f> corners;
        int maxVerts = (int)(1.0f / std::max(0.1f, settings.minSampleRadius) * 1000.0f);
        cv::goodFeaturesToTrack(gray, corners, maxVerts, 0.01, settings.minSampleRadius);
        return corners;
    }

    std::vector<PlotPath> AdaptiveCircularScribbles::generate(const cv::Mat& ref) {
        auto pts = generateAdaptivePoints(ref, settings);
        std::vector<PlotPath> paths;
        for(const auto& p : pts) { PlotPath path; path.points = {p, cv::Point2f(p.x+1, p.y+1), cv::Point2f(p.x-1, p.y+1), p}; paths.push_back(path); }
        return paths;
    }

    std::vector<PlotPath> AdaptiveShapes::generate(const cv::Mat& ref) {
        auto pts = generateAdaptivePoints(ref, settings);
        std::vector<PlotPath> paths;
        for (const auto& pt : pts) { PlotPath path; path.points = {cv::Point2f(pt.x-2, pt.y-2), cv::Point2f(pt.x+2, pt.y-2), cv::Point2f(pt.x+2, pt.y+2), cv::Point2f(pt.x-2, pt.y+2), cv::Point2f(pt.x-2, pt.y-2)}; paths.push_back(path); }
        return paths;
    }

    std::vector<PlotPath> AdaptiveTriangulation::generate(const cv::Mat& ref) {
        auto pts = generateAdaptivePoints(ref, settings);
        std::vector<PlotPath> paths;
        if(pts.empty()) return paths;
        
        cv::Rect bounds(0, 0, ref.cols, ref.rows);
        cv::Subdiv2D subdiv(bounds);
        for(const auto& pt : pts) if(bounds.contains(pt)) subdiv.insert(pt);
        std::vector<cv::Vec6f> triangles;
        subdiv.getTriangleList(triangles);
        for (const auto& t : triangles) {
            cv::Point2f p1(t[0], t[1]), p2(t[2], t[3]), p3(t[4], t[5]);
            if (bounds.contains(p1) && bounds.contains(p2) && bounds.contains(p3)) {
                PlotPath p; p.points = {p1, p2, p3, p1}; paths.push_back(p);
            }
        }
        return paths;
    }

    std::vector<PlotPath> AdaptiveTree::generate(const cv::Mat& ref) {
        auto pts = generateAdaptivePoints(ref, settings);
        if(pts.empty()) return {};
        std::vector<PlotPath> paths;
        std::vector<bool> inTree(pts.size(), false);
        inTree[0] = true;
        for(size_t i=1; i<pts.size(); i++) {
            float minDist = std::numeric_limits<float>::max();
            int bestU = -1, bestV = -1;
            for(size_t u=0; u<pts.size(); u++) {
                if(inTree[u]) {
                    for(size_t v=0; v<pts.size(); v++) {
                        if(!inTree[v]) {
                            float dist = std::pow(pts[u].x - pts[v].x, 2) + std::pow(pts[u].y - pts[v].y, 2);
                            if(dist < minDist) { minDist = dist; bestU = u; bestV = v; }
                        }
                    }
                }
            }
            if(bestU != -1 && bestV != -1) {
                inTree[bestV] = true;
                PlotPath p; p.points = {pts[bestU], pts[bestV]}; paths.push_back(p);
            }
        }
        return paths;
    }

    std::vector<PlotPath> AdaptiveStippling::generate(const cv::Mat& ref) {
        auto pts = generateAdaptivePoints(ref, settings);
        std::vector<PlotPath> paths;
        for (const auto& pt : pts) { PlotPath path; path.points = {pt, cv::Point2f(pt.x + 0.1f, pt.y)}; paths.push_back(path); }
        return paths;
    }

    std::vector<PlotPath> AdaptiveDashes::generate(const cv::Mat& ref) {
        auto pts = generateAdaptivePoints(ref, settings);
        std::vector<PlotPath> paths;
        for (const auto& pt : pts) { PlotPath path; path.points = {cv::Point2f(pt.x-3, pt.y), cv::Point2f(pt.x+3, pt.y)}; paths.push_back(path); }
        return paths;
    }

    std::vector<PlotPath> AdaptiveLetters::generate(const cv::Mat& ref) {
        auto pts = generateAdaptivePoints(ref, settings);
        std::vector<PlotPath> paths;
        for (const auto& pt : pts) { PlotPath path; path.points = {pt, cv::Point2f(pt.x+1, pt.y+1)}; paths.push_back(path); }
        return paths;
    }

    std::vector<PlotPath> AdaptiveDiagram::generate(const cv::Mat& ref) {
        auto pts = generateAdaptivePoints(ref, settings);
        std::vector<PlotPath> paths;
        if(pts.empty()) return paths;
        cv::Rect bounds(0, 0, ref.cols, ref.rows);
        cv::Subdiv2D subdiv(bounds);
        for(const auto& pt : pts) if(bounds.contains(pt)) subdiv.insert(pt);
        std::vector<std::vector<cv::Point2f>> facets;
        std::vector<cv::Point2f> centers;
        subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);
        for (const auto& facet : facets) {
            PlotPath p;
            for (const auto& fp : facet) p.points.push_back(fp);
            if (!p.points.empty()) p.points.push_back(p.points[0]);
            paths.push_back(p);
        }
        return paths;
    }
    
    std::vector<PlotPath> AdaptiveTSP::generate(const cv::Mat& ref) {
        auto pts = generateAdaptivePoints(ref, settings);
        if(pts.empty()) return {};
        std::vector<PlotPath> paths; PlotPath tsp;
        cv::Mat features(pts.size(), 2, CV_32F);
        for (size_t i = 0; i < pts.size(); i++) { features.at<float>(i, 0) = pts[i].x; features.at<float>(i, 1) = pts[i].y; }
        cv::flann::Index kdtree(features, cv::flann::KDTreeIndexParams(1));
        std::vector<bool> visited(pts.size(), false);
        
        cv::Point2f curr = pts[0]; visited[0] = true; tsp.points.push_back(curr);
        for (size_t i = 1; i < pts.size(); i++) {
            std::vector<int> indices(50); std::vector<float> dists(50);
            std::vector<float> query = { curr.x, curr.y };
            kdtree.knnSearch(query, indices, dists, std::min((int)pts.size(), 50));
            int bestIdx = -1;
            for(int idx : indices) if(!visited[idx]) { bestIdx = idx; break; }
            if(bestIdx == -1) for(size_t j=0; j<pts.size(); j++) if(!visited[j]) { bestIdx = j; break; }
            visited[bestIdx] = true; curr = pts[bestIdx]; tsp.points.push_back(curr);
        }
        paths.push_back(tsp); return paths;
    }
}
