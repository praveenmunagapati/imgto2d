#include "../include/PFM_Grid.h"
#include <opencv2/imgproc.hpp>
#include <cmath>
#include "../include/GeometryUtils.h"

namespace DrawingBot {
    static float getLuma(const cv::Vec3b& px) { return 0.299f * px[2] + 0.587f * px[1] + 0.114f * px[0]; }

    template<typename T>
    static std::vector<PlotPath> generateGrid(const cv::Mat& ref, const T& settings, int type) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        
        // Adjust brightness/contrast
        workImg.convertTo(workImg, -1, settings.contrast, (settings.brightness - 1.0f) * 128.0f);
        
        std::vector<PlotPath> paths;
        float gx = std::max(1.0f, settings.gridXSpacing);
        float gy = std::max(1.0f, settings.uniformSpacing ? settings.gridXSpacing : settings.gridYSpacing);
        float angleRad = 0.0f; // Could add rotation in settings later
        float cosA = std::cos(angleRad);
        float sinA = std::sin(angleRad);
        float diag = std::sqrt(ref.cols*ref.cols + ref.rows*ref.rows);
        
        for (float y = -diag; y < diag; y += gy) {
            bool oddRow = (int(y / gy) % 2) != 0;
            float xOffset = (settings.interleave && oddRow) ? (gx / 2.0f) : 0.0f;
            
            for (float x = -diag + xOffset; x < diag; x += gx) {
                float actX = x + ((rand() % 100) / 100.0f - 0.5f) * settings.randOffsetX;
                float actY = y + ((rand() % 100) / 100.0f - 0.5f) * settings.randOffsetY;
                
                int px = std::round(ref.cols/2.0f + actX*cosA - actY*sinA);
                int py = std::round(ref.rows/2.0f + actX*sinA + actY*cosA);
                
                if (px >= 0 && px < ref.cols && py >= 0 && py < ref.rows) {
                    float luma = getLuma(workImg.at<cv::Vec3b>(py, px));
                    float threshold = settings.threshold / 100.0f * 255.0f;
                    if (luma > threshold) continue;
                    
                    float scale = (255.0f - luma) / 255.0f * settings.shapeScale;
                    if (scale < 0.01f) continue;
                    
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
