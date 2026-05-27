#include "../include/PFM_Sketch.h"
#include <opencv2/imgproc.hpp>
#include <cmath>
#include <algorithm>
#include "../include/GeometryUtils.h"

namespace DrawingBot {
    
    static float getLuma(const cv::Vec3b& px) { return 0.299f * px[2] + 0.587f * px[1] + 0.114f * px[0]; }

    static std::vector<cv::Point2f> getSketchSeedPoints(const cv::Mat& ref, int count, float threshold) {
        std::vector<cv::Point2f> seeds;
        for (int i = 0; i < count; i++) {
            int sx = rand() % ref.cols;
            int sy = rand() % ref.rows;
            float luma = getLuma(ref.at<cv::Vec3b>(sy, sx));
            if (luma <= threshold) {
                seeds.push_back(cv::Point2f(sx, sy));
            }
        }
        return seeds;
    }

    std::vector<PlotPath> SketchLines::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        std::vector<PlotPath> paths;
        auto seeds = getSketchSeedPoints(workImg, settings.maxLines, settings.luminanceThreshold);
        for (const auto& seed : seeds) {
            PlotPath path;
            cv::Point2f curr = seed;
            float angle = (rand() % 360) * CV_PI / 180.0f;
            for (int j = 0; j < settings.lineLength; j++) {
                path.points.push_back(curr);
                angle += (rand() % 100 / 100.0f - 0.5f) * settings.curveSmoothness;
                curr.x += std::cos(angle) * 2.0f;
                curr.y += std::sin(angle) * 2.0f;
                if (curr.x < 0 || curr.x >= ref.cols || curr.y < 0 || curr.y >= ref.rows) break;
                if (getLuma(workImg.at<cv::Vec3b>((int)curr.y, (int)curr.x)) > settings.luminanceThreshold) break;
            }
            if (path.points.size() > 2) paths.push_back(path);
        }
        return paths;
    }

    std::vector<PlotPath> SketchCurves::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        std::vector<PlotPath> paths;
        auto seeds = getSketchSeedPoints(workImg, settings.maxLines, settings.luminanceThreshold);
        for (const auto& seed : seeds) {
            PlotPath path;
            cv::Point2f curr = seed;
            float angle = (rand() % 360) * CV_PI / 180.0f;
            for (int j = 0; j < settings.lineLength; j++) {
                path.points.push_back(curr);
                angle += curveTension * 0.5f; 
                curr.x += std::cos(angle) * 3.0f;
                curr.y += std::sin(angle) * 3.0f;
                if (curr.x < 0 || curr.x >= ref.cols || curr.y < 0 || curr.y >= ref.rows) break;
            }
            if (path.points.size() > 2) paths.push_back(path);
        }
        return paths;
    }

    std::vector<PlotPath> SketchSquares::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        std::vector<PlotPath> paths;
        auto seeds = getSketchSeedPoints(workImg, settings.maxLines, settings.luminanceThreshold);
        for (const auto& seed : seeds) {
            PlotPath path;
            float r = (rand() % 10) + 2.0f;
            path.points = {cv::Point2f(seed.x-r, seed.y-r), cv::Point2f(seed.x+r, seed.y-r), cv::Point2f(seed.x+r, seed.y+r), cv::Point2f(seed.x-r, seed.y+r), cv::Point2f(seed.x-r, seed.y-r)};
            paths.push_back(path);
        }
        return paths;
    }

    std::vector<PlotPath> SketchQuadBeziers::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        std::vector<PlotPath> paths;
        auto seeds = getSketchSeedPoints(workImg, settings.maxLines, settings.luminanceThreshold);
        for (const auto& seed : seeds) {
            PlotPath path;
            cv::Point2f p0 = seed;
            cv::Point2f p1(seed.x + curveOffset, seed.y + curveVariation);
            cv::Point2f p2(seed.x + curveVariation, seed.y + curveOffset);
            for (float t = 0; t <= 1.0f; t += 0.1f) {
                float u = 1.0f - t;
                cv::Point2f p = u*u*p0 + 2.0f*u*t*p1 + t*t*p2;
                path.points.push_back(p);
            }
            paths.push_back(path);
        }
        return paths;
    }

    std::vector<PlotPath> SketchCubicBeziers::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        std::vector<PlotPath> paths;
        auto seeds = getSketchSeedPoints(workImg, settings.maxLines, settings.luminanceThreshold);
        for (const auto& seed : seeds) {
            PlotPath path;
            cv::Point2f p0 = seed;
            cv::Point2f p1(seed.x + curveOffsetA, seed.y - curveOffsetB);
            cv::Point2f p2(seed.x - curveOffsetB, seed.y + curveOffsetA);
            cv::Point2f p3(seed.x + curveVariation, seed.y + curveVariation);
            for (float t = 0; t <= 1.0f; t += 0.1f) {
                float u = 1.0f - t;
                cv::Point2f p = u*u*u*p0 + 3.0f*u*u*t*p1 + 3.0f*u*t*t*p2 + t*t*t*p3;
                path.points.push_back(p);
            }
            paths.push_back(path);
        }
        return paths;
    }

    std::vector<PlotPath> SketchCatmullRoms::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        std::vector<PlotPath> paths;
        auto seeds = getSketchSeedPoints(workImg, settings.maxLines, settings.luminanceThreshold);
        for (const auto& seed : seeds) {
            PlotPath path;
            cv::Point2f p0(seed.x - 10, seed.y - 10);
            cv::Point2f p1 = seed;
            cv::Point2f p2(seed.x + 10, seed.y + 5);
            cv::Point2f p3(seed.x + 20, seed.y - 5);
            
            float alpha = 0.5f; // Centripetal
            float t0 = 0.0f;
            float t1 = t0 + std::pow(std::sqrt(std::pow(p1.x-p0.x, 2) + std::pow(p1.y-p0.y, 2)), alpha);
            float t2 = t1 + std::pow(std::sqrt(std::pow(p2.x-p1.x, 2) + std::pow(p2.y-p1.y, 2)), alpha);
            float t3 = t2 + std::pow(std::sqrt(std::pow(p3.x-p2.x, 2) + std::pow(p3.y-p2.y, 2)), alpha);

            for (float t = t1; t < t2; t += ((t2 - t1) / 10.0f)) {
                cv::Point2f a1 = (t1-t)/(t1-t0)*p0 + (t-t0)/(t1-t0)*p1;
                cv::Point2f a2 = (t2-t)/(t2-t1)*p1 + (t-t1)/(t2-t1)*p2;
                cv::Point2f a3 = (t3-t)/(t3-t2)*p2 + (t-t2)/(t3-t2)*p3;
                cv::Point2f b1 = (t2-t)/(t2-t0)*a1 + (t-t0)/(t2-t0)*a2;
                cv::Point2f b2 = (t3-t)/(t3-t1)*a2 + (t-t1)/(t3-t1)*a3;
                cv::Point2f c  = (t2-t)/(t2-t1)*b1 + (t-t1)/(t2-t1)*b2;
                path.points.push_back(c);
            }
            paths.push_back(path);
        }
        return paths;
    }

    std::vector<PlotPath> SketchShapes::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        std::vector<PlotPath> paths;
        auto seeds = getSketchSeedPoints(workImg, settings.maxLines, settings.luminanceThreshold);
        for (const auto& seed : seeds) {
            float r = (rand() % 10) + 2.0f;
            auto shapePaths = GeometryUtils::createShape(DrawingBot::ShapeType::SQUARE, seed, r);
            paths.insert(paths.end(), shapePaths.begin(), shapePaths.end());
        }
        return paths;
    }

    std::vector<PlotPath> SketchSobelEdges::generate(const cv::Mat& ref) {
        cv::Mat gray;
        if (ref.channels() == 3) cv::cvtColor(ref, gray, cv::COLOR_BGR2GRAY); else gray = ref.clone();
        cv::Mat grad_x, grad_y;
        cv::Sobel(gray, grad_x, CV_32F, 1, 0, 3);
        cv::Sobel(gray, grad_y, CV_32F, 0, 1, 3);
        
        std::vector<PlotPath> paths;
        auto seeds = getSketchSeedPoints(gray, settings.maxLines, settings.luminanceThreshold);
        for (const auto& seed : seeds) {
            PlotPath path;
            cv::Point2f curr = seed;
            for (int j = 0; j < settings.lineLength; j++) {
                path.points.push_back(curr);
                int cx = std::clamp((int)std::round(curr.x), 0, ref.cols - 1);
                int cy = std::clamp((int)std::round(curr.y), 0, ref.rows - 1);
                float gx = grad_x.at<float>(cy, cx);
                float gy = grad_y.at<float>(cy, cx);
                float mag = std::sqrt(gx*gx + gy*gy);
                if (mag < sobelAdjust) break;
                curr.x += (-gy / mag) * sobelIntensity;
                curr.y += (gx / mag) * sobelIntensity;
                if (curr.x < 0 || curr.x >= ref.cols || curr.y < 0 || curr.y >= ref.rows) break;
            }
            if (path.points.size() > 2) paths.push_back(path);
        }
        return paths;
    }

    std::vector<PlotPath> SketchWaves::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        std::vector<PlotPath> paths;
        auto seeds = getSketchSeedPoints(workImg, settings.maxLines, settings.luminanceThreshold);
        for (const auto& seed : seeds) {
            PlotPath path;
            cv::Point2f curr = seed;
            float angle = startAngle * CV_PI / 180.0f;
            for (int j = 0; j < settings.lineLength; j++) {
                float waveOffset = std::sin(curr.x / std::max(1.0f, waveDivisorX)) * waveOffsetX;
                path.points.push_back(cv::Point2f(curr.x, curr.y + waveOffset));
                
                curr.x += std::cos(angle) * 2.0f;
                curr.y += std::sin(angle) * 2.0f;
                if (curr.x < 0 || curr.x >= ref.cols || curr.y < 0 || curr.y >= ref.rows) break;
            }
            if (path.points.size() > 2) paths.push_back(path);
        }
        return paths;
    }

    std::vector<PlotPath> SketchFlowField::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        std::vector<PlotPath> paths;
        auto seeds = getSketchSeedPoints(workImg, settings.maxLines, settings.luminanceThreshold);
        for (const auto& seed : seeds) {
            PlotPath path;
            cv::Point2f curr = seed;
            for (int j = 0; j < settings.lineLength; j++) {
                path.points.push_back(curr);
                float angle = startAngle * CV_PI / 180.0f;
                angle += std::sin(curr.x * xFrequency * 0.01f) * amplitude;
                angle += std::cos(curr.y * yFrequency * 0.01f) * amplitude;
                
                curr.x += std::cos(angle) * 2.0f;
                curr.y += std::sin(angle) * 2.0f;
                if (curr.x < 0 || curr.x >= ref.cols || curr.y < 0 || curr.y >= ref.rows) break;
            }
            if (path.points.size() > 2) paths.push_back(path);
        }
        return paths;
    }

    std::vector<PlotPath> SketchSuperformula::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        std::vector<PlotPath> paths;
        auto seeds = getSketchSeedPoints(workImg, settings.maxLines, settings.luminanceThreshold);
        for (const auto& seed : seeds) {
            PlotPath path;
            cv::Point2f curr = seed;
            for (int j = 0; j < settings.lineLength; j++) {
                path.points.push_back(curr);
                float a = std::max(0.01f, xScale);
                float b = std::max(0.01f, yScale);
                float angle = std::atan2(curr.y - ref.rows/2, curr.x - ref.cols/2);
                float t1 = std::pow(std::abs(std::cos(frequency * angle / 4.0f) / a), sineFactor);
                float t2 = std::pow(std::abs(std::sin(frequency * angle / 4.0f) / b), cosFactor);
                float r = std::pow(t1 + t2, -1.0f / std::max(0.01f, curvature));
                
                curr.x += std::cos(angle + r) * 2.0f;
                curr.y += std::sin(angle + r) * 2.0f;
                if (curr.x < 0 || curr.x >= ref.cols || curr.y < 0 || curr.y >= ref.rows) break;
            }
            if (path.points.size() > 2) paths.push_back(path);
        }
        return paths;
    }

    std::vector<PlotPath> SketchSweepingCurves::generate(const cv::Mat& ref) {
        SketchCurves s; s.settings = settings; return s.generate(ref);
    }
}
