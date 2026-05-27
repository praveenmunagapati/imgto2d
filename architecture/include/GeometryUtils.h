#pragma once
#include "PFMBase.h"
#include <string>

namespace DrawingBot {
    
    // DBV3 Shape Types (approximated)
    enum class ShapeType {
        CIRCLE, SQUARE, TRIANGLE, DIAMOND, HEXAGON, OCTAGON, STAR
    };

    class GeometryUtils {
    public:
        // Returns a single path representing the outer contour of the shape
        static std::vector<PlotPath> createShape(ShapeType type, const cv::Point2f& center, float size);
        
        // Returns multiple paths representing the vector outlines of the text
        static std::vector<PlotPath> createText(const std::string& text, const cv::Point2f& position, float scale, const std::string& fontPath = "C:\\Windows\\Fonts\\arial.ttf");
        
        // Returns a simple dash centered at pt rotated by angleRad
        static std::vector<PlotPath> createDash(const cv::Point2f& pt, float size, float angleRad);
    };

} // namespace DrawingBot
