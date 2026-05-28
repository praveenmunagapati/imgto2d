#include "../include/PFM_Voronoi.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/flann.hpp>
#include <iostream>
#include <cmath>
#include "../include/GeometryUtils.h"

namespace DrawingBot {

    static float getLuma(const cv::Vec3b& px) {
        return 0.299f * px[2] + 0.587f * px[1] + 0.114f * px[0];
    }

    static std::vector<cv::Point2f> generateVoronoiPoints(const cv::Mat& ref,
        float pointDensity, int pointLimit, float luminancePower, float densityPower,
        int voronoiIterations, float voronoiAccuracy, bool ignoreWhite) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } 
        else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }

        int pointsCount = (int)(pointDensity);
        if (pointsCount <= 0) pointsCount = 100;
        if (pointsCount > pointLimit) pointsCount = pointLimit;
        
        std::vector<cv::Point2f> points;
        for (int i = 0; i < pointsCount; i++) {
            points.push_back(cv::Point2f(rand() % ref.cols, rand() % ref.rows));
        }

        cv::Rect bounds(0, 0, ref.cols, ref.rows);

        for (int iter = 0; iter < voronoiIterations; iter++) {
            cv::Subdiv2D subdiv(bounds);
            for (const auto& p : points) {
                if (bounds.contains(p)) subdiv.insert(p);
            }

            std::vector<std::vector<cv::Point2f>> facets;
            std::vector<cv::Point2f> centers;
            subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);

            std::vector<cv::Point2f> newPoints(points.size());

            for (size_t i = 0; i < facets.size(); i++) {
                if (i >= points.size()) break;

                double sumX = 0, sumY = 0, sumWeight = 0;
                
                std::vector<cv::Point> intFacet;
                for (auto& fp : facets[i]) intFacet.push_back(cv::Point(fp.x, fp.y));
                
                cv::Rect boundingBox = cv::boundingRect(intFacet) & bounds;
                
                for (int y = boundingBox.y; y < boundingBox.y + boundingBox.height; y+=2) {
                    for (int x = boundingBox.x; x < boundingBox.x + boundingBox.width; x+=2) {
                        if (cv::pointPolygonTest(facets[i], cv::Point2f(x, y), false) >= 0) {
                            float luma = getLuma(workImg.at<cv::Vec3b>(y, x));
                            static int printCount = 0;
                            if (printCount < 10) {
                                std::cout << "[DEBUG_LUMA] x=" << x << " y=" << y << " luma=" << luma << "\n";
                                printCount++;
                            }
                            float normLuma = luma / 255.0f;
                            float weight = 1.0f - normLuma;
                            weight = std::pow(weight, densityPower / 10.0f);
                            
                            sumX += x * weight;
                            sumY += y * weight;
                            sumWeight += weight;
                        }
                    }
                }

                if (sumWeight > 0.001) {
                    newPoints[i] = cv::Point2f(sumX / sumWeight, sumY / sumWeight);
                } else {
                    newPoints[i] = centers[i];
                }
            }
            points = newPoints;
        }
        return points;
    }

    // Convenience overload for VoronoiBaseSettings
    static std::vector<cv::Point2f> generateVoronoiPointsFromBase(const cv::Mat& ref, const VoronoiBaseSettings& s) {
        return generateVoronoiPoints(ref, s.pointDensity, s.pointLimit, s.luminancePower,
            s.densityPower, s.voronoiIterations, s.voronoiAccuracy, s.ignoreWhite);
    }

    std::vector<PlotPath> VoronoiShapes::generate(const cv::Mat& ref) {
        auto pts = generateVoronoiPointsFromBase(ref, settings);
        std::vector<PlotPath> paths;
        for (const auto& pt : pts) {
            auto shapePaths = GeometryUtils::createShape(DrawingBot::ShapeType::CIRCLE, pt, settings.fillSize);
            paths.insert(paths.end(), shapePaths.begin(), shapePaths.end());
        }
        return paths;
    }

    std::vector<PlotPath> VoronoiTriangulation::generate(const cv::Mat& ref) {
        auto pts = generateVoronoiPointsFromBase(ref, settings);
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

    std::vector<PlotPath> VoronoiTree::generate(const cv::Mat& ref) {
        auto pts = generateVoronoiPointsFromBase(ref, settings);
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

    std::vector<PlotPath> VoronoiTSP::generate(const cv::Mat& ref) {
        auto pts = generateVoronoiPointsFromBase(ref, settings);
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

    std::vector<PlotPath> VoronoiStippling::generate(const cv::Mat& ref) {
        auto pts = generateVoronoiPointsFromBase(ref, settings);
        std::vector<PlotPath> paths;
        for (const auto& pt : pts) { PlotPath path; path.points = {pt, cv::Point2f(pt.x + settings.stippleSize * 0.1f, pt.y)}; paths.push_back(path); }
        return paths;
    }

    std::vector<PlotPath> VoronoiDashes::generate(const cv::Mat& ref) {
        auto pts = generateVoronoiPointsFromBase(ref, settings);
        std::vector<PlotPath> paths;
        cv::Mat gray;
        if (ref.channels() == 3) cv::cvtColor(ref, gray, cv::COLOR_BGR2GRAY);
        else gray = ref.clone();
        cv::Mat grad_x, grad_y;
        cv::Sobel(gray, grad_x, CV_32F, 1, 0, 3);
        cv::Sobel(gray, grad_y, CV_32F, 0, 1, 3);
        for (const auto& pt : pts) {
            int cx = std::clamp((int)std::round(pt.x), 0, ref.cols - 1);
            int cy = std::clamp((int)std::round(pt.y), 0, ref.rows - 1);
            float gx = grad_x.at<float>(cy, cx);
            float gy = grad_y.at<float>(cy, cx);
            float angle = std::atan2(gy, gx) + CV_PI / 2.0f;
            auto dashPaths = GeometryUtils::createDash(pt, settings.fillSize, angle);
            paths.insert(paths.end(), dashPaths.begin(), dashPaths.end());
        }
        return paths;
    }

    std::vector<PlotPath> VoronoiLetters::generate(const cv::Mat& ref) {
        auto pts = generateVoronoiPointsFromBase(ref, settings);
        std::vector<PlotPath> paths;
        for (const auto& pt : pts) {
            auto textPaths = GeometryUtils::createText("A", pt, 0.05f);
            paths.insert(paths.end(), textPaths.begin(), textPaths.end());
        }
        return paths;
    }

    std::vector<PlotPath> VoronoiDiagram::generate(const cv::Mat& ref) {
        auto pts = generateVoronoiPointsFromBase(ref, settings);
        std::vector<PlotPath> paths;
        cv::Rect bounds(0, 0, ref.cols, ref.rows);
        cv::Subdiv2D subdiv(bounds);
        for(const auto& pt : pts) if(bounds.contains(pt)) subdiv.insert(pt);
        std::vector<std::vector<cv::Point2f>> facets;
        std::vector<cv::Point2f> centers;
        subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);
        for (const auto& facet : facets) {
            PlotPath p;
            for (const auto& fp : facet) p.points.push_back(fp);
            if (!p.points.empty()) p.points.push_back(p.points[0]); // close
            paths.push_back(p);
        }
        return paths;
    }

}
