#include "../include/PFM_Streamline.h"
#include <opencv2/imgproc.hpp>
#include <cmath>
#include <algorithm>

namespace DrawingBot {

    static float getLuma(const cv::Vec3b& px) { return 0.299f * px[2] + 0.587f * px[1] + 0.114f * px[0]; }

    // Edge Field using Sobel derivatives
    std::vector<PlotPath> StreamlinesEdgeField::generate(const cv::Mat& ref) {
        cv::Mat gray;
        if (ref.channels() == 3) cv::cvtColor(ref, gray, cv::COLOR_BGR2GRAY); else gray = ref.clone();
        
        cv::GaussianBlur(gray, gray, cv::Size(7, 7), settings.flowFieldSmoothness);
        cv::Mat grad_x, grad_y;
        cv::Sobel(gray, grad_x, CV_32F, 1, 0, 3);
        cv::Sobel(gray, grad_y, CV_32F, 0, 1, 3);
        
        std::vector<PlotPath> paths;
        float step = std::max(0.1f, settings.stepSize);
        float dSep = std::max(1.0f, settings.dSep);
        
        for (float y = 0; y < ref.rows; y += dSep) {
            for (float x = 0; x < ref.cols; x += dSep) {
                PlotPath path;
                cv::Point2f curr(x, y);
                for (int i = 0; i < settings.maxStreamlineLength; i++) {
                    path.points.push_back(curr);
                    int cx = std::clamp((int)std::round(curr.x), 0, ref.cols - 1);
                    int cy = std::clamp((int)std::round(curr.y), 0, ref.rows - 1);
                    
                    float gx = grad_x.at<float>(cy, cx);
                    float gy = grad_y.at<float>(cy, cx);
                    float mag = std::sqrt(gx*gx + gy*gy);
                    if (mag < 0.1f) break;
                    
                    curr.x += (-gy / mag) * step;
                    curr.y += (gx / mag) * step;
                    
                    if (curr.x < 0 || curr.x >= ref.cols || curr.y < 0 || curr.y >= ref.rows) break;
                }
                if (path.points.size() > 2) paths.push_back(path);
            }
        }
        return paths;
    }

    // Procedural Flow Field using perlin/sine waves
    std::vector<PlotPath> StreamlinesFlowField::generate(const cv::Mat& ref) {
        std::vector<PlotPath> paths;
        float step = std::max(0.1f, settings.stepSize);
        float dSep = std::max(1.0f, settings.dSep);
        
        for (float y = 0; y < ref.rows; y += dSep) {
            for (float x = 0; x < ref.cols; x += dSep) {
                PlotPath path;
                cv::Point2f curr(x, y);
                for (int i = 0; i < settings.maxStreamlineLength; i++) {
                    path.points.push_back(curr);
                    
                    float angle = startAngle * CV_PI / 180.0f;
                    angle += std::sin(curr.x * xFrequency * 0.01f) * amplitude;
                    angle += std::cos(curr.y * yFrequency * 0.01f) * amplitude;
                    
                    curr.x += std::cos(angle) * step;
                    curr.y += std::sin(angle) * step;
                    
                    if (curr.x < 0 || curr.x >= ref.cols || curr.y < 0 || curr.y >= ref.rows) break;
                }
                if (path.points.size() > 2) paths.push_back(path);
            }
        }
        return paths;
    }

    // Superformula mapping
    std::vector<PlotPath> StreamlinesSuperformula::generate(const cv::Mat& ref) {
        std::vector<PlotPath> paths;
        float step = std::max(0.1f, settings.stepSize);
        float dSep = std::max(1.0f, settings.dSep);
        
        for (float y = 0; y < ref.rows; y += dSep) {
            for (float x = 0; x < ref.cols; x += dSep) {
                PlotPath path;
                cv::Point2f curr(x, y);
                for (int i = 0; i < settings.maxStreamlineLength; i++) {
                    path.points.push_back(curr);
                    
                    float a = std::max(0.01f, xScale);
                    float b = std::max(0.01f, yScale);
                    float m = frequency;
                    float n1 = curvature;
                    float n2 = sineFactor;
                    float n3 = cosFactor;
                    
                    float angle = std::atan2(curr.y - ref.rows/2, curr.x - ref.cols/2);
                    float t1 = std::pow(std::abs(std::cos(m * angle / 4.0f) / a), n2);
                    float t2 = std::pow(std::abs(std::sin(m * angle / 4.0f) / b), n3);
                    float r = std::pow(t1 + t2, -1.0f / n1);
                    
                    curr.x += std::cos(angle + r) * step;
                    curr.y += std::sin(angle + r) * step;
                    
                    if (curr.x < 0 || curr.x >= ref.cols || curr.y < 0 || curr.y >= ref.rows) break;
                }
                if (path.points.size() > 2) paths.push_back(path);
            }
        }
        return paths;
    }
}
