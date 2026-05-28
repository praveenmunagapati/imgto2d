#include "../include/PFM_Hatch.h"
#include <opencv2/imgproc.hpp>
#include <cmath>
#include <algorithm>

namespace DrawingBot {

    static float getLuma(const cv::Vec3b& px) { return 0.299f * px[2] + 0.587f * px[1] + 0.114f * px[0]; }

    std::vector<PlotPath> HatchSawtooth::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        
        float spacing = std::max(2.0f, settings.lineSpacing);
        float angle = settings.angle * CV_PI / 180.0f;
        float dx = std::cos(angle), dy = std::sin(angle);
        float nx = -dy, ny = dx;
        
        std::vector<PlotPath> paths;
        float maxDim = std::sqrt((float)(ref.cols*ref.cols + ref.rows*ref.rows));
        
        for (float offset = -maxDim; offset < maxDim; offset += spacing) {
            PlotPath path;
            for (float t = 0; t < maxDim * 2; t += 2.0f) {
                float px = ref.cols / 2.0f + dx * (t - maxDim) + nx * offset;
                float py = ref.rows / 2.0f + dy * (t - maxDim) + ny * offset;
                
                if (px >= 0 && px < ref.cols && py >= 0 && py < ref.rows) {
                    float luma = getLuma(workImg.at<cv::Vec3b>((int)py, (int)px));
                    if (luma < settings.amplitude * 255.0f * 2.0f) {
                        float vel = settings.minVelocity + (luma / 255.0f) * (settings.maxVelocity - settings.minVelocity);
                        float sawAmp = (255.0f - luma) / 255.0f * spacing * 0.4f;
                        float sawVal = sawAmp * std::sin(t * vel * CV_PI / 180.0f);
                        path.points.push_back(cv::Point2f(px + nx * sawVal, py + ny * sawVal));
                    }
                }
            }
            if (path.points.size() > 2) paths.push_back(path);
        }
        
        if (settings.crosshatch) {
            float crossAngle = (settings.angle + 90.0f) * CV_PI / 180.0f;
            float cdx = std::cos(crossAngle), cdy = std::sin(crossAngle);
            float cnx = -cdy, cny = cdx;
            for (float offset = -maxDim; offset < maxDim; offset += spacing) {
                PlotPath path;
                for (float t = 0; t < maxDim * 2; t += 2.0f) {
                    float px = ref.cols / 2.0f + cdx * (t - maxDim) + cnx * offset;
                    float py = ref.rows / 2.0f + cdy * (t - maxDim) + cny * offset;
                    if (px >= 0 && px < ref.cols && py >= 0 && py < ref.rows) {
                        float luma = getLuma(workImg.at<cv::Vec3b>((int)py, (int)px));
                        if (luma < 128.0f) {
                            path.points.push_back(cv::Point2f(px, py));
                        }
                    }
                }
                if (path.points.size() > 2) paths.push_back(path);
            }
        }
        return paths;
    }

    std::vector<PlotPath> HatchCircularScribbles::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        
        float spacing = std::max(2.0f, settings.lineSpacing);
        float angle = settings.angle * CV_PI / 180.0f;
        float dx = std::cos(angle), dy = std::sin(angle);
        float nx = -dy, ny = dx;
        
        std::vector<PlotPath> paths;
        float maxDim = std::sqrt((float)(ref.cols*ref.cols + ref.rows*ref.rows));
        float loopTheta = 0.0f;
        
        for (float offset = -maxDim; offset < maxDim; offset += spacing) {
            PlotPath path;
            for (float t = 0; t < maxDim * 2; t += 2.0f) {
                float px = ref.cols / 2.0f + dx * (t - maxDim) + nx * offset;
                float py = ref.rows / 2.0f + dy * (t - maxDim) + ny * offset;
                
                if (px >= 0 && px < ref.cols && py >= 0 && py < ref.rows) {
                    float luma = getLuma(workImg.at<cv::Vec3b>((int)py, (int)px));
                    float loopR = settings.minRadius + (luma / 255.0f) * (settings.maxRadius - settings.minRadius);
                    loopTheta += settings.angularVelocity * CV_PI / 180.0f * 0.1f;
                    float wx = px + loopR * std::cos(loopTheta);
                    float wy = py + loopR * std::sin(loopTheta);
                    path.points.push_back(cv::Point2f(wx, wy));
                }
            }
            if (path.points.size() > 2) paths.push_back(path);
        }
        return paths;
    }
}
