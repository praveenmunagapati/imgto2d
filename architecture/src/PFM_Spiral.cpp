#include "../include/PFM_Spiral.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

namespace DrawingBot {
    
    static float getLuma(const cv::Vec3b& px) { return 0.299f * px[2] + 0.587f * px[1] + 0.114f * px[0]; }

    std::vector<PlotPath> SpiralSawtooth::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        
        PlotPath path;
        float maxR = std::min((float)ref.cols, settings.maxRadius);
        float cx = ref.cols * settings.originX;
        float cy = ref.rows * settings.originY;
        
        for (float theta = 0; theta < 500.0f; theta += 0.1f) {
            float r = settings.pitch * theta;
            if (r > maxR) break;
            
            float angle = settings.clockwise ? theta : -theta;
            int x = std::round(cx + r * std::cos(angle));
            int y = std::round(cy + r * std::sin(angle));
            
            if (x >= 0 && x < ref.cols && y >= 0 && y < ref.rows) {
                float luma = getLuma(workImg.at<cv::Vec3b>(y, x));
                float amp = (255.0f - luma) / 255.0f * settings.pitch;
                
                // Sawtooth logic
                float phase = std::fmod(theta * 5.0f, 1.0f);
                float wobbledR = r + (phase - 0.5f) * amp;
                
                float finalX = cx + wobbledR * std::cos(angle);
                float finalY = cy + wobbledR * std::sin(angle);
                
                path.points.push_back(cv::Point2f(finalX, finalY));
            }
        }
        return { path };
    }

    std::vector<PlotPath> SpiralCircularScribbles::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        
        std::vector<PlotPath> paths;
        float maxR = std::min((float)ref.cols, settings.maxRadius);
        float cx = ref.cols * settings.originX;
        float cy = ref.rows * settings.originY;
        
        for (float theta = 0; theta < 500.0f; theta += 1.0f) {
            float r = settings.pitch * theta;
            if (r > maxR) break;
            
            float angle = settings.clockwise ? theta : -theta;
            int x = std::round(cx + r * std::cos(angle));
            int y = std::round(cy + r * std::sin(angle));
            
            if (x >= 0 && x < ref.cols && y >= 0 && y < ref.rows) {
                float luma = getLuma(workImg.at<cv::Vec3b>(y, x));
                float radius = (255.0f - luma) / 255.0f * settings.pitch * 0.5f;
                
                if (radius > 1.0f) {
                    PlotPath scribble;
                    for (float a = 0; a < 6.28f; a += 0.5f) {
                        scribble.points.push_back(cv::Point2f(x + radius * std::cos(a), y + radius * std::sin(a)));
                    }
                    scribble.points.push_back(scribble.points.front());
                    paths.push_back(scribble);
                }
            }
        }
        return paths;
    }
}
