#include "../include/PFM_Spiral.h"
#include <opencv2/imgproc.hpp>
#include <cmath>
#include <algorithm>

namespace DrawingBot {

    static float getLuma(const cv::Vec3b& px) { return 0.299f * px[2] + 0.587f * px[1] + 0.114f * px[0]; }

    std::vector<PlotPath> SpiralSawtooth::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        
        float maxR = std::min((float)ref.cols, (float)ref.rows) * settings.spiralSize / 100.0f;
        float cx = ref.cols * settings.centreX / 100.0f;
        float cy = ref.rows * settings.centreY / 100.0f;
        
        PlotPath path;
        for (float theta = 0; ; theta += 0.05f) {
            float r = settings.ringSpacing * theta;
            if (r > maxR) break;
            
            float x = cx + r * std::cos(theta);
            float y = cy + r * std::sin(theta);
            
            if (x >= 0 && x < ref.cols && y >= 0 && y < ref.rows) {
                float luma = getLuma(workImg.at<cv::Vec3b>((int)y, (int)x));
                float amp = (255.0f - luma) / 255.0f * settings.amplitude * settings.ringSpacing;
                x += std::cos(theta + CV_PI/2) * amp * std::sin(theta * 
                    (settings.variableVelocity ? (settings.minVelocity + luma/255.0f * (settings.maxVelocity - settings.minVelocity)) : settings.minVelocity) * CV_PI / 180.0f);
                y += std::sin(theta + CV_PI/2) * amp * std::sin(theta *
                    (settings.variableVelocity ? (settings.minVelocity + luma/255.0f * (settings.maxVelocity - settings.minVelocity)) : settings.minVelocity) * CV_PI / 180.0f);
            }
            path.points.push_back(cv::Point2f(x, y));
        }
        std::vector<PlotPath> paths;
        if (path.points.size() > 2) paths.push_back(path);
        return paths;
    }

    std::vector<PlotPath> SpiralCircularScribbles::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        
        float maxR = std::min((float)ref.cols, (float)ref.rows) * settings.spiralSize / 100.0f;
        float cx = ref.cols * settings.centreX / 100.0f;
        float cy = ref.rows * settings.centreY / 100.0f;
        
        PlotPath path;
        float loopTheta = 0.0f;
        for (float theta = 0; ; theta += 0.05f) {
            float r = settings.ringSpacing * theta;
            if (r > maxR) break;
            
            float x = cx + r * std::cos(theta);
            float y = cy + r * std::sin(theta);
            
            if (x >= 0 && x < ref.cols && y >= 0 && y < ref.rows) {
                float luma = getLuma(workImg.at<cv::Vec3b>((int)y, (int)x));
                float loopR = settings.minRadius + (luma / 255.0f) * (settings.maxRadius - settings.minRadius);
                loopTheta += settings.angularVelocity * CV_PI / 180.0f * 0.05f;
                x += loopR * std::cos(loopTheta);
                y += loopR * std::sin(loopTheta);
            }
            path.points.push_back(cv::Point2f(x, y));
        }
        std::vector<PlotPath> paths;
        if (path.points.size() > 2) paths.push_back(path);
        return paths;
    }
}
