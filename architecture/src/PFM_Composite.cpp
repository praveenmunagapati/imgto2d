#include "../include/PFM_Composite.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/flann.hpp>

namespace DrawingBot {

    // Helper to run nested styles
    static void generateNestedPaths(const cv::Mat& ref, const std::vector<cv::Point2f>& poly, const std::vector<DrawingStyle>& styles, std::vector<PlotPath>& outPaths) {
        if (styles.empty() || !styles[0].pfm) return;

        cv::Mat mask = cv::Mat::zeros(ref.size(), CV_8UC1);
        std::vector<std::vector<cv::Point>> pts(1);
        for (const auto& p : poly) pts[0].push_back(cv::Point(std::round(p.x), std::round(p.y)));
        cv::fillPoly(mask, pts, cv::Scalar(255));

        cv::Mat cellImg(ref.size(), ref.type(), cv::Scalar::all(255)); // White background so algorithms ignore outside
        ref.copyTo(cellImg, mask);

        auto nestedPaths = styles[0].pfm->generate(cellImg);
        
        // Optional: strict clipping of paths to the polygon could happen here, but white-background usually handles it.
        outPaths.insert(outPaths.end(), nestedPaths.begin(), nestedPaths.end());
    }

    std::vector<PlotPath> MosaicRectangles::generate(const cv::Mat& ref) {
        std::vector<PlotPath> paths;
        int cols = columns > 0 ? columns : 10;
        int rws = rows > 0 ? rows : 10;
        float w = (float)ref.cols / cols;
        float h = (float)ref.rows / rws;
        
        for (int y = 0; y < rws; y++) {
            for (int x = 0; x < cols; x++) {
                float px = x * w;
                float py = y * h;
                std::vector<cv::Point2f> poly = {
                    cv::Point2f(px, py), cv::Point2f(px+w, py),
                    cv::Point2f(px+w, py+h), cv::Point2f(px, py+h)
                };
                
                generateNestedPaths(ref, poly, drawingStyles, paths);

                if (drawOutlines) {
                    PlotPath path;
                    path.points = poly;
                    path.points.push_back(poly.front());
                    paths.push_back(path);
                }
            }
        }
        return paths;
    }

    std::vector<PlotPath> MosaicVoronoi::generate(const cv::Mat& ref) {
        std::vector<PlotPath> paths;
        int pointCount = tileCount > 0 ? tileCount : 100;
        cv::Rect bounds(0, 0, ref.cols, ref.rows);
        cv::Subdiv2D subdiv(bounds);
        
        for (int i = 0; i < pointCount; i++) {
            subdiv.insert(cv::Point2f(rand() % ref.cols, rand() % ref.rows));
        }
        
        std::vector<std::vector<cv::Point2f>> facets;
        std::vector<cv::Point2f> centers;
        subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);
        
        for (const auto& facet : facets) {
            generateNestedPaths(ref, facet, drawingStyles, paths);

            if (drawOutlines && !facet.empty()) {
                PlotPath path;
                path.points = facet;
                path.points.push_back(facet.front());
                paths.push_back(path);
            }
        }
        return paths;
    }

    std::vector<PlotPath> MosaicTriangulation::generate(const cv::Mat& ref) {
        std::vector<PlotPath> paths;
        int pointCount = tileCount > 0 ? tileCount : 100;
        cv::Rect bounds(0, 0, ref.cols, ref.rows);
        cv::Subdiv2D subdiv(bounds);
        
        for (int i = 0; i < pointCount; i++) {
            subdiv.insert(cv::Point2f(rand() % ref.cols, rand() % ref.rows));
        }
        
        std::vector<cv::Vec6f> triangles;
        subdiv.getTriangleList(triangles);
        for (const auto& t : triangles) {
            cv::Point2f p1(t[0], t[1]), p2(t[2], t[3]), p3(t[4], t[5]);
            if (bounds.contains(p1) && bounds.contains(p2) && bounds.contains(p3)) {
                std::vector<cv::Point2f> poly = {p1, p2, p3};
                generateNestedPaths(ref, poly, drawingStyles, paths);

                if (drawOutlines) {
                    PlotPath p; p.points = poly; p.points.push_back(p1); paths.push_back(p);
                }
            }
        }
        return paths;
    }

    std::vector<PlotPath> MosaicSegments::generate(const cv::Mat& ref) {
        return MosaicTriangulation().generate(ref);
    }

    std::vector<PlotPath> MosaicCustom::generate(const cv::Mat& ref) {
        return MosaicVoronoi().generate(ref);
    }

    std::vector<PlotPath> LayersPFM::generate(const cv::Mat& ref) {
        std::vector<PlotPath> paths;
        for (const auto& style : drawingStyles) {
            if (style.pfm) {
                auto layerPaths = style.pfm->generate(ref);
                paths.insert(paths.end(), layerPaths.begin(), layerPaths.end());
            }
        }
        return paths;
    }
}
