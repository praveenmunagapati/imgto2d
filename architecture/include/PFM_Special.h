#pragma once
#include "PFMBase.h"

namespace DrawingBot {

    class ECSDrawing : public PFMBase {
    public:
        // Edges
        bool  drawEdges;
        float edgeBlur;         // 0-50
        int   edgeDetail;       // 0-255
        float edgeSimplify;     // 0-100
        float edgeDistortion;   // 0-100
        
        // Contours
        bool  drawContours;
        float contourBlur;      // 0-99
        int   contourDetail;    // 0-255
        float contourSimplify;  // 0-100
        float contourDistortion;// 0-100
        
        // Shading
        bool  drawShading;
        float shadingAccuracy;  // 0-100
        float shadingDetail;    // 0-100
        float shadingLength;    // 0-150
        
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class SVGConverter : public PFMBase {
    public:
        std::string svgPath;
        bool  shapeClipping;
        bool  deriveDrawingSet;
        bool  shapeFilling;
        float spacing;          // 1.0-10.0
        float minRotation;      // 0-360
        float maxRotation;      // 0-360
        bool  linkEnds;
        bool  crosshatch;
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

    class PenCalibration : public PFMBase {
    public:
        float       nibSizeMin;  // 0.1-2.0
        float       nibSizeMax;  // 0.1-2.0
        int         testCount;   // 0-100
        float       testSize;    // 10.0-40.0 (mm)
        float       spacingX;    // 0-40.0 (mm)
        float       spacingY;    // 0-40.0 (mm)
        float       rotation;    // 0-360
        bool        lineTests;
        bool        circleTests;
        std::string svgFont;
        std::string title;
        float       fontSize;    // 1.0-8.0 (mm)
        std::vector<PlotPath> generate(const cv::Mat& ref) override;
    };

} // namespace DrawingBot
