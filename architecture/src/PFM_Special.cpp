#include "../include/PFM_Special.h"
#include <opencv2/imgproc.hpp>
#include <cmath>
#include <fstream>
#include <sstream>
#include <regex>

namespace DrawingBot {

    std::vector<PlotPath> ECSDrawing::generate(const cv::Mat& ref) {
        cv::Mat gray;
        if (ref.channels() == 3) cv::cvtColor(ref, gray, cv::COLOR_BGR2GRAY); else gray = ref.clone();
        
        cv::Mat edges;
        cv::Canny(gray, edges, 50, 150);
        
        std::vector<PlotPath> paths;
        for (int y = 0; y < edges.rows; y++) {
            for (int x = 0; x < edges.cols; x++) {
                if (edges.at<uchar>(y, x) > 128) {
                    PlotPath p;
                    p.points.push_back(cv::Point2f(x, y));
                    p.points.push_back(cv::Point2f(x+1, y+1));
                    paths.push_back(p);
                }
            }
        }
        return paths;
    }

    std::vector<PlotPath> SVGConverter::generate(const cv::Mat& ref) {
        // Fallback rudimentary parser that looks for basic paths
        std::vector<PlotPath> paths;
        if(settings.svgPath.empty()) return paths;
        
        std::ifstream file(settings.svgPath);
        if(!file.is_open()) return paths;
        
        std::string line;
        std::regex pathRegex("d=\"([^\"]+)\"");
        std::smatch match;
        
        while(std::getline(file, line)) {
            if(std::regex_search(line, match, pathRegex)) {
                std::string d = match[1].str();
                std::stringstream ss(d);
                char cmd;
                float x, y;
                PlotPath p;
                while(ss >> cmd >> x >> y) {
                    if(cmd == 'M' || cmd == 'L') {
                        p.points.push_back(cv::Point2f(x, y));
                    }
                }
                if(p.points.size() > 1) paths.push_back(p);
            }
        }
        return paths;
    }
    
    std::vector<PlotPath> PenCalibration::generate(const cv::Mat& ref) {
        std::vector<PlotPath> paths;
        float cx = ref.cols / 2.0f;
        float cy = ref.rows / 2.0f;
        for(int i = 0; i < settings.testCount; i++) {
            PlotPath p;
            float r = settings.testSize + (i * settings.spacingX);
            for(float a = 0; a < 6.28f; a += 0.1f) {
                p.points.push_back(cv::Point2f(cx + r * std::cos(a), cy + r * std::sin(a)));
            }
            paths.push_back(p);
        }
        return paths;
    }
}
