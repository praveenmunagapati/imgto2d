#include "../include/PFM_Grid.h"
#include <opencv2/imgproc.hpp>
#include <cmath>
#include "../include/GeometryUtils.h"

namespace DrawingBot {
    static float getLuma(const cv::Vec3b& px) { return 0.299f * px[2] + 0.587f * px[1] + 0.114f * px[0]; }

    static std::vector<PlotPath> generateGrid(const cv::Mat& ref, const GridSettings& settings, int type) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        
        std::vector<PlotPath> paths;
        float gx = std::max(2.0f, settings.gridSizeX);
        float gy = std::max(2.0f, settings.gridSizeY);
        float angleRad = settings.angle * CV_PI / 180.0f;
        float cosA = std::cos(angleRad);
        float sinA = std::sin(angleRad);
        float diag = std::sqrt(ref.cols*ref.cols + ref.rows*ref.rows);
        
        for (float y = -diag; y < diag; y += gy) {
            for (float x = -diag; x < diag; x += gx) {
                int px = std::round(ref.cols/2.0f + x*cosA - y*sinA);
                int py = std::round(ref.rows/2.0f + x*sinA + y*cosA);
                
                if (px >= 0 && px < ref.cols && py >= 0 && py < ref.rows) {
                    float luma = getLuma(workImg.at<cv::Vec3b>(py, px));
                    float scale = (255.0f - luma) / 255.0f * settings.cellScale;
                    if (scale < 0.05f) continue;
                    
                    float hw = (gx * scale) / 2.0f;
                    float hh = (gy * scale) / 2.0f;
                    
                    if (type == 0) { // Box/Shape
                        auto shapePaths = GeometryUtils::createShape(ShapeType::SQUARE, cv::Point2f(px, py), std::max(hw, hh));
                        paths.insert(paths.end(), shapePaths.begin(), shapePaths.end());
                    } else if (type == 1) { // Dashes
                        // Simple dash aligned with grid angle
                        auto dashPaths = GeometryUtils::createDash(cv::Point2f(px, py), std::max(gx, gy) * scale, angleRad);
                        paths.insert(paths.end(), dashPaths.begin(), dashPaths.end());
                    } else if (type == 2) { // Letters
                        auto textPaths = GeometryUtils::createText("G", cv::Point2f(px - hw/2, py + hh/2), scale * 0.05f); // Example char
                        paths.insert(paths.end(), textPaths.begin(), textPaths.end());
                    }
                }
            }
        }
        return paths;
    }

    std::vector<PlotPath> GridShapes::generate(const cv::Mat& ref) { return generateGrid(ref, settings, 0); }
    std::vector<PlotPath> GridDashes::generate(const cv::Mat& ref) { return generateGrid(ref, settings, 1); }
    std::vector<PlotPath> GridLetters::generate(const cv::Mat& ref) { return generateGrid(ref, settings, 2); }
}
