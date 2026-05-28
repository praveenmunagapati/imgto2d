#include "../include/PathOptimizer.h"
#include <cmath>
#include <algorithm>
#include <iostream>

namespace DrawingBot {

    static float perpendicularDistance(const cv::Point2f& pt, const cv::Point2f& lineStart, const cv::Point2f& lineEnd) {
        float dx = lineEnd.x - lineStart.x;
        float dy = lineEnd.y - lineStart.y;
        float mag = std::sqrt(dx*dx + dy*dy);
        if (mag > 0.0f) {
            dx /= mag; dy /= mag;
        }
        float pvx = pt.x - lineStart.x;
        float pvy = pt.y - lineStart.y;
        float pvdot = pvx * dx + pvy * dy;
        float dsx = pvdot * dx;
        float dsy = pvdot * dy;
        float ax = pvx - dsx;
        float ay = pvy - dsy;
        return std::sqrt(ax*ax + ay*ay);
    }

    static void rdp(const std::vector<cv::Point2f>& pointList, float epsilon, std::vector<cv::Point2f>& out) {
        if (pointList.size() < 2) {
            out = pointList;
            return;
        }

        float dmax = 0.0f;
        size_t index = 0;
        size_t end = pointList.size() - 1;

        for (size_t i = 1; i < end; i++) {
            float d = perpendicularDistance(pointList[i], pointList[0], pointList[end]);
            if (d > dmax) {
                index = i;
                dmax = d;
            }
        }

        if (dmax > epsilon) {
            std::vector<cv::Point2f> recResults1;
            std::vector<cv::Point2f> recResults2;
            std::vector<cv::Point2f> firstLine(pointList.begin(), pointList.begin() + index + 1);
            std::vector<cv::Point2f> lastLine(pointList.begin() + index, pointList.end());
            
            rdp(firstLine, epsilon, recResults1);
            rdp(lastLine, epsilon, recResults2);
            
            out.assign(recResults1.begin(), recResults1.end() - 1);
            out.insert(out.end(), recResults2.begin(), recResults2.end());
        } else {
            out.push_back(pointList[0]);
            out.push_back(pointList[end]);
        }
    }

    void PathOptimizer::simplifyPaths(std::vector<PlotPath>& paths, float epsilon) {
        for (auto& path : paths) {
            if (path.points.size() > 2) {
                std::vector<cv::Point2f> simplified;
                rdp(path.points, epsilon, simplified);
                path.points = simplified;
            }
        }
    }

    void PathOptimizer::joinPaths(std::vector<PlotPath>& paths, float joinDistance) {
        if (paths.empty()) return;
        
        std::vector<PlotPath> joined;
        joined.push_back(paths[0]);
        
        for (size_t i = 1; i < paths.size(); i++) {
            if (paths[i].points.empty()) continue;
            
            auto& lastPath = joined.back();
            if (lastPath.points.empty()) {
                joined.push_back(paths[i]);
                continue;
            }
            
            cv::Point2f p1 = lastPath.points.back();
            cv::Point2f p2 = paths[i].points.front();
            
            float dist = std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
            if (dist <= joinDistance) {
                lastPath.points.insert(lastPath.points.end(), paths[i].points.begin(), paths[i].points.end());
            } else {
                joined.push_back(paths[i]);
            }
        }
        paths = joined;
    }

    void PathOptimizer::optimizeRouting(std::vector<PlotPath>& paths) {
        if (paths.size() <= 1) return;
        
        std::vector<PlotPath> optimized;
        optimized.reserve(paths.size());
        
        std::vector<bool> visited(paths.size(), false);
        
        // Start with the first path
        optimized.push_back(paths[0]);
        visited[0] = true;
        
        for (size_t i = 1; i < paths.size(); i++) {
            cv::Point2f currEnd = optimized.back().points.back();
            float minDist = std::numeric_limits<float>::max();
            int bestIdx = -1;
            bool reverseBest = false;
            
            for (size_t j = 0; j < paths.size(); j++) {
                if (visited[j] || paths[j].points.empty()) continue;
                
                cv::Point2f pStart = paths[j].points.front();
                cv::Point2f pEnd = paths[j].points.back();
                
                float d1 = std::pow(currEnd.x - pStart.x, 2) + std::pow(currEnd.y - pStart.y, 2);
                float d2 = std::pow(currEnd.x - pEnd.x, 2) + std::pow(currEnd.y - pEnd.y, 2);
                
                if (d1 < minDist) { minDist = d1; bestIdx = j; reverseBest = false; }
                if (d2 < minDist) { minDist = d2; bestIdx = j; reverseBest = true; }
            }
            
            if (bestIdx != -1) {
                visited[bestIdx] = true;
                PlotPath nextPath = paths[bestIdx];
                if (reverseBest) {
                    std::reverse(nextPath.points.begin(), nextPath.points.end());
                }
                optimized.push_back(nextPath);
            }
        }
        
        paths = optimized;
    }

} // namespace DrawingBot
