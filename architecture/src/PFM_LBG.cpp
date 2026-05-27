#include "../include/PFM_LBG.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/flann.hpp>
#include <iostream>
#include <cmath>

namespace DrawingBot {

    static float getLuma(const cv::Vec3b& px) {
        return 0.299f * px[2] + 0.587f * px[1] + 0.114f * px[0];
    }

    static std::vector<cv::Point2f> generateLBGPoints(const cv::Mat& ref, const LBGSettings& settings) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } 
        else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }

        // Rejection sampling to build dataset for K-Means
        std::vector<cv::Point2f> samples;
        int maxSamples = settings.clusterCount * 10;
        if (maxSamples < 1000) maxSamples = 1000;
        if (maxSamples > 100000) maxSamples = 100000;
        
        for (int i = 0; i < maxSamples; i++) {
            int x = rand() % ref.cols;
            int y = rand() % ref.rows;
            float luma = getLuma(workImg.at<cv::Vec3b>(y, x));
            float normLuma = luma / 255.0f;
            float weight = (1.0f - normLuma);
            if ((rand() % 1000) / 1000.0f < std::pow(weight, 2.0f)) {
                samples.push_back(cv::Point2f(x, y));
            }
        }
        
        if (samples.size() < settings.clusterCount) {
            // Not enough samples, just return them
            return samples;
        }

        // Run K-Means (Linde-Buzo-Gray)
        cv::Mat sampleMat(samples.size(), 2, CV_32F);
        for(size_t i=0; i<samples.size(); i++) {
            sampleMat.at<float>(i, 0) = samples[i].x;
            sampleMat.at<float>(i, 1) = samples[i].y;
        }
        
        cv::Mat labels;
        cv::Mat centers;
        cv::kmeans(sampleMat, settings.clusterCount, labels, 
                   cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, settings.maxIterations, settings.minError),
                   3, cv::KMEANS_PP_CENTERS, centers);
                   
        std::vector<cv::Point2f> finalPoints;
        for (int i=0; i<centers.rows; i++) {
            finalPoints.push_back(cv::Point2f(centers.at<float>(i, 0), centers.at<float>(i, 1)));
        }
        return finalPoints;
    }

    std::vector<PlotPath> LBGCircularScribbles::generate(const cv::Mat& ref) {
        auto pts = generateLBGPoints(ref, settings);
        std::vector<PlotPath> paths;
        for(const auto& p : pts) { PlotPath path; path.points = {p, cv::Point2f(p.x+1, p.y+1), cv::Point2f(p.x-1, p.y+1), p}; paths.push_back(path); }
        return paths;
    }

    std::vector<PlotPath> LBGShapes::generate(const cv::Mat& ref) {
        auto pts = generateLBGPoints(ref, settings);
        std::vector<PlotPath> paths;
        for (const auto& pt : pts) { PlotPath path; path.points = {cv::Point2f(pt.x-2, pt.y-2), cv::Point2f(pt.x+2, pt.y-2), cv::Point2f(pt.x+2, pt.y+2), cv::Point2f(pt.x-2, pt.y+2), cv::Point2f(pt.x-2, pt.y-2)}; paths.push_back(path); }
        return paths;
    }

    std::vector<PlotPath> LBGTriangulation::generate(const cv::Mat& ref) {
        auto pts = generateLBGPoints(ref, settings);
        std::vector<PlotPath> paths;
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

    std::vector<PlotPath> LBGTree::generate(const cv::Mat& ref) {
        auto pts = generateLBGPoints(ref, settings);
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

    std::vector<PlotPath> LBGStippling::generate(const cv::Mat& ref) {
        auto pts = generateLBGPoints(ref, settings);
        std::vector<PlotPath> paths;
        for (const auto& pt : pts) { PlotPath path; path.points = {pt, cv::Point2f(pt.x + 0.1f, pt.y)}; paths.push_back(path); }
        return paths;
    }

    std::vector<PlotPath> LBGDashes::generate(const cv::Mat& ref) {
        auto pts = generateLBGPoints(ref, settings);
        std::vector<PlotPath> paths;
        for (const auto& pt : pts) { PlotPath path; path.points = {cv::Point2f(pt.x-3, pt.y), cv::Point2f(pt.x+3, pt.y)}; paths.push_back(path); }
        return paths;
    }

    std::vector<PlotPath> LBGLetters::generate(const cv::Mat& ref) {
        auto pts = generateLBGPoints(ref, settings);
        std::vector<PlotPath> paths;
        for (const auto& pt : pts) { PlotPath path; path.points = {pt, cv::Point2f(pt.x+1, pt.y+1)}; paths.push_back(path); }
        return paths;
    }

    std::vector<PlotPath> LBGDiagram::generate(const cv::Mat& ref) { LBGTree t; t.settings = settings; return t.generate(ref); }
    
    std::vector<PlotPath> LBGTSP::generate(const cv::Mat& ref) {
        auto pts = generateLBGPoints(ref, settings);
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
    
    std::vector<PlotPath> LBGQuadTiles::generate(const cv::Mat& ref) {
        auto pts = generateLBGPoints(ref, settings);
        std::vector<PlotPath> paths;
        for (const auto& pt : pts) { PlotPath path; path.points = {cv::Point2f(pt.x-2, pt.y-2), cv::Point2f(pt.x+2, pt.y-2), cv::Point2f(pt.x+2, pt.y+2), cv::Point2f(pt.x-2, pt.y+2), cv::Point2f(pt.x-2, pt.y-2)}; paths.push_back(path); }
        return paths;
    }

}
