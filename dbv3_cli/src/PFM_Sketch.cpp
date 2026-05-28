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
            float luma = (ref.channels() == 3) ? getLuma(ref.at<cv::Vec3b>(sy, sx)) : (float)ref.at<uchar>(sy, sx);
            if (luma <= threshold) {
                seeds.push_back(cv::Point2f(sx, sy));
            }
        }
        return seeds;
    }

    // Helper: convert line density (0-100%) to a raw line count
    static int lineCountFromDensity(float lineDensity, int imgArea) {
        return std::max(10, (int)(lineDensity / 100.0f * imgArea / 50.0f));
    }

    // ── SketchLines ─────────────────────────────────────────────────────
    std::vector<PlotPath> SketchLines::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        std::vector<PlotPath> paths;
        int count = lineCountFromDensity(settings.lineDensity, ref.cols * ref.rows);
        if (settings.lineMaxLimit > 0 && count > settings.lineMaxLimit) count = settings.lineMaxLimit;
        float threshold = settings.tone / 100.0f * 255.0f;
        auto seeds = getSketchSeedPoints(workImg, count, threshold);
        for (const auto& seed : seeds) {
            PlotPath path;
            cv::Point2f curr = seed;
            float angle = (settings.startAngleMin + (rand() % (int)std::max(1.0f, settings.startAngleMax - settings.startAngleMin))) * CV_PI / 180.0f;
            int len = settings.lineMinLength + rand() % std::max(1, settings.lineMaxLength - settings.lineMinLength);
            for (int j = 0; j < len; j++) {
                path.points.push_back(curr);
                angle += (rand() % 100 / 100.0f - 0.5f) * (settings.angularity / 50.0f);
                angle += settings.drawingDeltaAngle * CV_PI / 180.0f * 0.01f;
                curr.x += std::cos(angle) * 2.0f;
                curr.y += std::sin(angle) * 2.0f;
                if (curr.x < 0 || curr.x >= ref.cols || curr.y < 0 || curr.y >= ref.rows) break;
                if (getLuma(workImg.at<cv::Vec3b>((int)curr.y, (int)curr.x)) > threshold) break;
            }
            if (path.points.size() > 2) paths.push_back(path);
        }
        return paths;
    }

    // ── SketchCurves ────────────────────────────────────────────────────
    std::vector<PlotPath> SketchCurves::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        std::vector<PlotPath> paths;
        int count = lineCountFromDensity(settings.lineDensity, ref.cols * ref.rows);
        float threshold = settings.tone / 100.0f * 255.0f;
        auto seeds = getSketchSeedPoints(workImg, count, threshold);
        for (const auto& seed : seeds) {
            PlotPath path;
            cv::Point2f curr = seed;
            float angle = (rand() % 360) * CV_PI / 180.0f;
            int len = settings.lineMinLength + rand() % std::max(1, settings.lineMaxLength - settings.lineMinLength);
            for (int j = 0; j < len; j++) {
                path.points.push_back(curr);
                angle += settings.curveTension * 0.5f;
                curr.x += std::cos(angle) * 3.0f;
                curr.y += std::sin(angle) * 3.0f;
                if (curr.x < 0 || curr.x >= ref.cols || curr.y < 0 || curr.y >= ref.rows) break;
            }
            if (path.points.size() > 2) paths.push_back(path);
        }
        return paths;
    }

    // ── SketchSquares ───────────────────────────────────────────────────
    std::vector<PlotPath> SketchSquares::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        std::vector<PlotPath> paths;
        int count = lineCountFromDensity(settings.lineDensity, ref.cols * ref.rows);
        float threshold = settings.tone / 100.0f * 255.0f;
        auto seeds = getSketchSeedPoints(workImg, count, threshold);
        for (const auto& seed : seeds) {
            PlotPath path;
            float r = (rand() % 10) + 2.0f;
            float a = settings.startAngle * CV_PI / 180.0f;
            float ca = std::cos(a), sa = std::sin(a);
            // Rotated square
            cv::Point2f corners[4] = {
                {seed.x + (-r*ca - -r*sa), seed.y + (-r*sa + -r*ca)},
                {seed.x + (r*ca - -r*sa),  seed.y + (r*sa + -r*ca)},
                {seed.x + (r*ca - r*sa),   seed.y + (r*sa + r*ca)},
                {seed.x + (-r*ca - r*sa),  seed.y + (-r*sa + r*ca)}
            };
            path.points = {corners[0], corners[1], corners[2], corners[3], corners[0]};
            paths.push_back(path);
        }
        return paths;
    }

    // ── SketchQuadBeziers ───────────────────────────────────────────────
    std::vector<PlotPath> SketchQuadBeziers::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        std::vector<PlotPath> paths;
        int count = lineCountFromDensity(settings.lineDensity, ref.cols * ref.rows);
        float threshold = settings.tone / 100.0f * 255.0f;
        auto seeds = getSketchSeedPoints(workImg, count, threshold);
        for (const auto& seed : seeds) {
            PlotPath path;
            cv::Point2f p0 = seed;
            cv::Point2f p1(seed.x + settings.curveOffset, seed.y + settings.curveVariation);
            cv::Point2f p2(seed.x + settings.curveVariation, seed.y + settings.curveOffset);
            for (float t = 0; t <= 1.0f; t += 0.1f) {
                float u = 1.0f - t;
                cv::Point2f p = u*u*p0 + 2.0f*u*t*p1 + t*t*p2;
                path.points.push_back(p);
            }
            paths.push_back(path);
        }
        return paths;
    }

    // ── SketchCubicBeziers ──────────────────────────────────────────────
    std::vector<PlotPath> SketchCubicBeziers::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        std::vector<PlotPath> paths;
        int count = lineCountFromDensity(settings.lineDensity, ref.cols * ref.rows);
        float threshold = settings.tone / 100.0f * 255.0f;
        auto seeds = getSketchSeedPoints(workImg, count, threshold);
        for (const auto& seed : seeds) {
            PlotPath path;
            cv::Point2f p0 = seed;
            cv::Point2f p1(seed.x + settings.curveOffsetA, seed.y - settings.curveOffsetB);
            cv::Point2f p2(seed.x - settings.curveOffsetB, seed.y + settings.curveOffsetA);
            cv::Point2f p3(seed.x + settings.curveVariation, seed.y + settings.curveVariation);
            for (float t = 0; t <= 1.0f; t += 0.1f) {
                float u = 1.0f - t;
                cv::Point2f p = u*u*u*p0 + 3.0f*u*u*t*p1 + 3.0f*u*t*t*p2 + t*t*t*p3;
                path.points.push_back(p);
            }
            paths.push_back(path);
        }
        return paths;
    }

    // ── SketchCatmullRoms ───────────────────────────────────────────────
    std::vector<PlotPath> SketchCatmullRoms::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        std::vector<PlotPath> paths;
        int count = lineCountFromDensity(settings.lineDensity, ref.cols * ref.rows);
        float threshold = settings.tone / 100.0f * 255.0f;
        auto seeds = getSketchSeedPoints(workImg, count, threshold);
        for (const auto& seed : seeds) {
            PlotPath path;
            cv::Point2f p0(seed.x - 10, seed.y - 10);
            cv::Point2f p1 = seed;
            cv::Point2f p2(seed.x + 10, seed.y + 5);
            cv::Point2f p3(seed.x + 20, seed.y - 5);
            float alpha = settings.curveTension;
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

    // ── SketchShapes ────────────────────────────────────────────────────
    std::vector<PlotPath> SketchShapes::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        std::vector<PlotPath> paths;
        int count = lineCountFromDensity(settings.lineDensity, ref.cols * ref.rows);
        float threshold = settings.tone / 100.0f * 255.0f;
        auto seeds = getSketchSeedPoints(workImg, count, threshold);
        ShapeType stype = (settings.shapeType == SketchShapesSettings::ShapeType::ELLIPSES)
            ? ShapeType::CIRCLE : ShapeType::SQUARE;
        for (const auto& seed : seeds) {
            float r = (rand() % 10) + 2.0f;
            auto shapePaths = GeometryUtils::createShape(stype, seed, r);
            paths.insert(paths.end(), shapePaths.begin(), shapePaths.end());
        }
        return paths;
    }

    // ── SketchSobelEdges ────────────────────────────────────────────────
    std::vector<PlotPath> SketchSobelEdges::generate(const cv::Mat& ref) {
        cv::Mat gray;
        if (ref.channels() == 3) cv::cvtColor(ref, gray, cv::COLOR_BGR2GRAY); else gray = ref.clone();
        cv::Mat grad_x, grad_y;
        cv::Sobel(gray, grad_x, CV_32F, 1, 0, 3);
        cv::Sobel(gray, grad_y, CV_32F, 0, 1, 3);
        
        std::vector<PlotPath> paths;
        int count = lineCountFromDensity(settings.lineDensity, ref.cols * ref.rows);
        float threshold = settings.tone / 100.0f * 255.0f;
        auto seeds = getSketchSeedPoints(gray, count, threshold);
        for (const auto& seed : seeds) {
            PlotPath path;
            cv::Point2f curr = seed;
            int len = settings.lineMinLength + rand() % std::max(1, settings.lineMaxLength - settings.lineMinLength);
            for (int j = 0; j < len; j++) {
                path.points.push_back(curr);
                int cx = std::clamp((int)std::round(curr.x), 0, ref.cols - 1);
                int cy = std::clamp((int)std::round(curr.y), 0, ref.rows - 1);
                float gx = grad_x.at<float>(cy, cx);
                float gy = grad_y.at<float>(cy, cx);
                float mag = std::sqrt(gx*gx + gy*gy);
                if (mag < settings.sobelAdjust) break;
                curr.x += (-gy / mag) * settings.sobelIntensity;
                curr.y += (gx / mag) * settings.sobelIntensity;
                if (curr.x < 0 || curr.x >= ref.cols || curr.y < 0 || curr.y >= ref.rows) break;
            }
            if (path.points.size() > 2) paths.push_back(path);
        }
        return paths;
    }

    // ── SketchWaves ─────────────────────────────────────────────────────
    std::vector<PlotPath> SketchWaves::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        std::vector<PlotPath> paths;
        int count = lineCountFromDensity(settings.lineDensity, ref.cols * ref.rows);
        float threshold = settings.tone / 100.0f * 255.0f;
        auto seeds = getSketchSeedPoints(workImg, count, threshold);
        for (const auto& seed : seeds) {
            PlotPath path;
            cv::Point2f curr = seed;
            float angle = settings.startAngle * CV_PI / 180.0f;
            int len = settings.lineMinLength + rand() % std::max(1, settings.lineMaxLength - settings.lineMinLength);
            for (int j = 0; j < len; j++) {
                float vX = curr.x / std::max(1.0f, settings.waveDivisorX);
                float vY = curr.y / std::max(1.0f, settings.waveDivisorY);
                
                float waveOffsetXVal = 0.0f;
                if (settings.waveTypeX == SketchWavesSettings::WaveType::SIN) waveOffsetXVal = std::sin(vX) * settings.waveOffsetX;
                else if (settings.waveTypeX == SketchWavesSettings::WaveType::COS) waveOffsetXVal = std::cos(vX) * settings.waveOffsetX;
                else if (settings.waveTypeX == SketchWavesSettings::WaveType::TAN) waveOffsetXVal = std::tan(vX) * settings.waveOffsetX;
                
                float waveOffsetYVal = 0.0f;
                if (settings.waveTypeY == SketchWavesSettings::WaveType::SIN) waveOffsetYVal = std::sin(vY) * settings.waveOffsetY;
                else if (settings.waveTypeY == SketchWavesSettings::WaveType::COS) waveOffsetYVal = std::cos(vY) * settings.waveOffsetY;
                else if (settings.waveTypeY == SketchWavesSettings::WaveType::TAN) waveOffsetYVal = std::tan(vY) * settings.waveOffsetY;
                
                path.points.push_back(cv::Point2f(curr.x + waveOffsetXVal, curr.y + waveOffsetYVal));
                
                curr.x += std::cos(angle) * 2.0f;
                curr.y += std::sin(angle) * 2.0f;
                if (curr.x < 0 || curr.x >= ref.cols || curr.y < 0 || curr.y >= ref.rows) break;
            }
            if (path.points.size() > 2) paths.push_back(path);
        }
        return paths;
    }

    // ── SketchFlowField ─────────────────────────────────────────────────
    std::vector<PlotPath> SketchFlowField::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        std::vector<PlotPath> paths;
        int count = lineCountFromDensity(settings.lineDensity, ref.cols * ref.rows);
        float threshold = settings.tone / 100.0f * 255.0f;
        auto seeds = getSketchSeedPoints(workImg, count, threshold);
        for (const auto& seed : seeds) {
            PlotPath path;
            cv::Point2f curr = seed;
            int len = settings.lineMinLength + rand() % std::max(1, settings.lineMaxLength - settings.lineMinLength);
            for (int j = 0; j < len; j++) {
                path.points.push_back(curr);
                float angle = settings.startAngle * CV_PI / 180.0f;
                angle += std::sin(curr.x * settings.xFrequency * 0.01f) * settings.amplitude;
                angle += std::cos(curr.y * settings.yFrequency * 0.01f) * settings.amplitude;
                
                curr.x += std::cos(angle) * 2.0f;
                curr.y += std::sin(angle) * 2.0f;
                if (curr.x < 0 || curr.x >= ref.cols || curr.y < 0 || curr.y >= ref.rows) break;
            }
            if (path.points.size() > 2) paths.push_back(path);
        }
        return paths;
    }

    // ── SketchSuperformula ──────────────────────────────────────────────
    std::vector<PlotPath> SketchSuperformula::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        std::vector<PlotPath> paths;
        int count = lineCountFromDensity(settings.lineDensity, ref.cols * ref.rows);
        float threshold = settings.tone / 100.0f * 255.0f;
        auto seeds = getSketchSeedPoints(workImg, count, threshold);
        for (const auto& seed : seeds) {
            PlotPath path;
            cv::Point2f curr = seed;
            int len = settings.lineMinLength + rand() % std::max(1, settings.lineMaxLength - settings.lineMinLength);
            for (int j = 0; j < len; j++) {
                path.points.push_back(curr);
                float a = std::max(0.01f, settings.xScale);
                float b = std::max(0.01f, settings.yScale);
                float angle = std::atan2(curr.y - ref.rows/2, curr.x - ref.cols/2);
                float t1 = std::pow(std::abs(std::cos(settings.frequency * angle / 4.0f) / a), settings.sineFactor);
                float t2 = std::pow(std::abs(std::sin(settings.frequency * angle / 4.0f) / b), settings.cosFactor);
                float r = std::pow(t1 + t2, -1.0f / std::max(0.01f, settings.curvature));
                
                curr.x += std::cos(angle + r) * 2.0f;
                curr.y += std::sin(angle + r) * 2.0f;
                if (curr.x < 0 || curr.x >= ref.cols || curr.y < 0 || curr.y >= ref.rows) break;
            }
            if (path.points.size() > 2) paths.push_back(path);
        }
        return paths;
    }

    // ── SketchSweepingCurves ────────────────────────────────────────────
    std::vector<PlotPath> SketchSweepingCurves::generate(const cv::Mat& ref) {
        cv::Mat workImg;
        if (ref.channels() == 3) { workImg = ref.clone(); } else { cv::cvtColor(ref, workImg, cv::COLOR_GRAY2BGR); }
        std::vector<PlotPath> paths;
        int count = lineCountFromDensity(settings.lineDensity, ref.cols * ref.rows);
        float threshold = settings.tone / 100.0f * 255.0f;
        auto seeds = getSketchSeedPoints(workImg, count, threshold);
        for (const auto& seed : seeds) {
            PlotPath path;
            cv::Point2f curr = seed;
            float angle = (rand() % 360) * CV_PI / 180.0f;
            int len = settings.lineMinLength + rand() % std::max(1, settings.lineMaxLength - settings.lineMinLength);
            for (int j = 0; j < len; j++) {
                path.points.push_back(curr);
                angle += settings.curvature * 0.2f;
                curr.x += std::cos(angle) * 3.0f;
                curr.y += std::sin(angle) * 3.0f;
                if (curr.x < 0 || curr.x >= ref.cols || curr.y < 0 || curr.y >= ref.rows) break;
            }
            if (path.points.size() > 2) paths.push_back(path);
        }
        return paths;
    }
}
