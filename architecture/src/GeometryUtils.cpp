#include "../include/GeometryUtils.h"
#include <cmath>
#include <fstream>
#include <iostream>

#define STB_TRUETYPE_IMPLEMENTATION
#include "../include/stb_truetype.h"

namespace DrawingBot {

    std::vector<PlotPath> GeometryUtils::createDash(const cv::Point2f& pt, float size, float angleRad) {
        std::vector<PlotPath> paths;
        float dx = std::cos(angleRad) * (size / 2.0f);
        float dy = std::sin(angleRad) * (size / 2.0f);
        PlotPath p;
        p.points = { cv::Point2f(pt.x - dx, pt.y - dy), cv::Point2f(pt.x + dx, pt.y + dy) };
        paths.push_back(p);
        return paths;
    }

    std::vector<PlotPath> GeometryUtils::createShape(ShapeType type, const cv::Point2f& center, float size) {
        std::vector<PlotPath> paths;
        PlotPath path;
        int sides = 4;
        float startAngle = 0.0f;
        
        switch (type) {
            case ShapeType::CIRCLE: sides = 16; break;
            case ShapeType::SQUARE: sides = 4; startAngle = CV_PI / 4.0f; break;
            case ShapeType::TRIANGLE: sides = 3; startAngle = -CV_PI / 2.0f; break;
            case ShapeType::DIAMOND: sides = 4; break;
            case ShapeType::HEXAGON: sides = 6; break;
            case ShapeType::OCTAGON: sides = 8; break;
            case ShapeType::STAR: sides = 10; startAngle = -CV_PI / 2.0f; break;
        }

        for (int i = 0; i < sides; i++) {
            float angle = startAngle + (i * 2.0f * CV_PI / sides);
            float r = size;
            if (type == ShapeType::STAR && i % 2 != 0) {
                r = size * 0.4f; // Inner radius for star
            }
            path.points.push_back(cv::Point2f(center.x + r * std::cos(angle), center.y + r * std::sin(angle)));
        }
        path.points.push_back(path.points.front()); // Close path
        paths.push_back(path);
        return paths;
    }

    std::vector<PlotPath> GeometryUtils::createText(const std::string& text, const cv::Point2f& position, float scale, const std::string& fontPath) {
        std::vector<PlotPath> paths;
        
        std::ifstream file(fontPath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) return paths; // Fallback silently if font fails
        
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::vector<unsigned char> fontBuffer(size);
        if (!file.read((char*)fontBuffer.data(), size)) return paths;
        
        stbtt_fontinfo font;
        if (!stbtt_InitFont(&font, fontBuffer.data(), 0)) return paths;
        
        float xpos = position.x;
        float ypos = position.y;
        
        for (char c : text) {
            int advance, lsb;
            stbtt_GetCodepointHMetrics(&font, c, &advance, &lsb);
            
            stbtt_vertex *vertices;
            int num_verts = stbtt_GetCodepointShape(&font, c, &vertices);
            
            PlotPath currentPath;
            for (int i = 0; i < num_verts; i++) {
                float vx = xpos + vertices[i].x * scale;
                float vy = ypos - vertices[i].y * scale; // Invert Y for stbtt
                
                if (vertices[i].type == STBTT_vmove) {
                    if (currentPath.points.size() > 1) { paths.push_back(currentPath); }
                    currentPath.points.clear();
                    currentPath.points.push_back(cv::Point2f(vx, vy));
                } else if (vertices[i].type == STBTT_vline) {
                    currentPath.points.push_back(cv::Point2f(vx, vy));
                } else if (vertices[i].type == STBTT_vcurve) {
                    // Simple quadratic bezier subdivision for plotter
                    cv::Point2f p0 = currentPath.points.back();
                    cv::Point2f p1(xpos + vertices[i].cx * scale, ypos - vertices[i].cy * scale);
                    cv::Point2f p2(vx, vy);
                    for (float t = 0.2f; t <= 1.0f; t += 0.2f) {
                        float u = 1.0f - t;
                        currentPath.points.push_back(u*u*p0 + 2.0f*u*t*p1 + t*t*p2);
                    }
                } else if (vertices[i].type == STBTT_vcubic) {
                    // Simple cubic bezier subdivision
                    cv::Point2f p0 = currentPath.points.back();
                    cv::Point2f p1(xpos + vertices[i].cx * scale, ypos - vertices[i].cy * scale);
                    cv::Point2f p2(xpos + vertices[i].cx1 * scale, ypos - vertices[i].cy1 * scale);
                    cv::Point2f p3(vx, vy);
                    for (float t = 0.2f; t <= 1.0f; t += 0.2f) {
                        float u = 1.0f - t;
                        currentPath.points.push_back(u*u*u*p0 + 3.0f*u*u*t*p1 + 3.0f*u*t*t*p2 + t*t*t*p3);
                    }
                }
            }
            if (currentPath.points.size() > 1) paths.push_back(currentPath);
            stbtt_FreeShape(&font, vertices);
            xpos += advance * scale;
        }
        
        return paths;
    }

} // namespace DrawingBot
