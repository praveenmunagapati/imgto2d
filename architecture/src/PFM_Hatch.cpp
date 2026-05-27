#include "../include/PFM_Hatch.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

namespace DrawingBot {
    static float getLuma(const cv::Vec3b& px) { return 0.299f * px[2] + 0.587f * px[1] + 0.114f * px[0]; }

    static std::vector<PlotPath> generateHatchLines(const cv::Mat& ref, const HatchSettings& settings) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        
        std::vector<PlotPath> paths;
        float spacing = std::max(0.1f, settings.hatchSpacing);
        float diag = std::sqrt(ref.cols*ref.cols + ref.rows*ref.rows);
        
        for (int level = 0; level < settings.hatchLevels; level++) {
            float currentSpacing = spacing * (level + 1);
            float angleDeg = settings.hatchAngle;
            if (settings.enableCrossHatch && level % 2 != 0) angleDeg = settings.crossHatchAngle;
            
            float angleRad = angleDeg * CV_PI / 180.0f;
            float cosA = std::cos(angleRad);
            float sinA = std::sin(angleRad);
            
            for (float d = -diag; d < diag; d += currentSpacing) {
                PlotPath currentPath;
                bool drawing = false;
                
                for (float t = -diag; t < diag; t += 1.0f) {
                    int x = std::round(ref.cols/2.0f + d*cosA - t*sinA);
                    int y = std::round(ref.rows/2.0f + d*sinA + t*cosA);
                    
                    if (x >= 0 && x < ref.cols && y >= 0 && y < ref.rows) {
                        float luma = getLuma(workImg.at<cv::Vec3b>(y, x));
                        float threshold = 255.0f * (1.0f - (float)(level + 1) / (settings.hatchLevels + 1));
                        
                        if (luma < threshold) {
                            if (!drawing) { drawing = true; currentPath = PlotPath(); }
                            currentPath.points.push_back(cv::Point2f(x, y));
                        } else {
                            if (drawing) { paths.push_back(currentPath); drawing = false; }
                        }
                    } else {
                        if (drawing) { paths.push_back(currentPath); drawing = false; }
                    }
                }
                if (drawing) paths.push_back(currentPath);
            }
        }
        return paths;
    }

    std::vector<PlotPath> HatchSawtooth::generate(const cv::Mat& ref) { return generateHatchLines(ref, settings); }
    std::vector<PlotPath> HatchCircularScribbles::generate(const cv::Mat& ref) { auto s = settings; s.enableCrossHatch = true; return generateHatchLines(ref, s); }
}
