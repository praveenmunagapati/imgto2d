#include "filters/morph_filters.h"
#include <opencv2/imgproc.hpp>

// ---------------------------------------------------------------------------
// Dilation & Erosion
// ---------------------------------------------------------------------------
QVector<PFMSetting> DilationFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 3, QVariant(), 1, 31, 1, 31, 2 },
        { "iterations", "Iterations", SettingType::Integer, 1, QVariant(), 1, 10, 1, 10, 1 }
    };
}
cv::Mat DilationFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    int iters = get("iterations").toInt();
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(ksize, ksize));
    cv::Mat out;
    cv::dilate(image, out, kernel, cv::Point(-1,-1), iters);
    return out;
}

QVector<PFMSetting> ErosionFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 3, QVariant(), 1, 31, 1, 31, 2 },
        { "iterations", "Iterations", SettingType::Integer, 1, QVariant(), 1, 10, 1, 10, 1 }
    };
}
cv::Mat ErosionFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    int iters = get("iterations").toInt();
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(ksize, ksize));
    cv::Mat out;
    cv::erode(image, out, kernel, cv::Point(-1,-1), iters);
    return out;
}

// ---------------------------------------------------------------------------
// Advanced Morph
// ---------------------------------------------------------------------------
QVector<PFMSetting> OpeningFilter::defineSettings() const {
    return { { "ksize", "Kernel Size", SettingType::Integer, 3, QVariant(), 1, 31, 1, 31, 2 } };
}
cv::Mat OpeningFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(ksize, ksize));
    cv::Mat out;
    cv::morphologyEx(image, out, cv::MORPH_OPEN, kernel);
    return out;
}

QVector<PFMSetting> ClosingFilter::defineSettings() const {
    return { { "ksize", "Kernel Size", SettingType::Integer, 3, QVariant(), 1, 31, 1, 31, 2 } };
}
cv::Mat ClosingFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(ksize, ksize));
    cv::Mat out;
    cv::morphologyEx(image, out, cv::MORPH_CLOSE, kernel);
    return out;
}

QVector<PFMSetting> MorphGradientFilter::defineSettings() const {
    return { { "ksize", "Kernel Size", SettingType::Integer, 3, QVariant(), 1, 31, 1, 31, 2 } };
}
cv::Mat MorphGradientFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(ksize, ksize));
    cv::Mat out;
    cv::morphologyEx(image, out, cv::MORPH_GRADIENT, kernel);
    return out;
}

QVector<PFMSetting> TopHatFilter::defineSettings() const {
    return { { "ksize", "Kernel Size", SettingType::Integer, 9, QVariant(), 1, 101, 1, 101, 2 } };
}
cv::Mat TopHatFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(ksize, ksize));
    cv::Mat out;
    cv::morphologyEx(image, out, cv::MORPH_TOPHAT, kernel);
    return out;
}

QVector<PFMSetting> BlackHatFilter::defineSettings() const {
    return { { "ksize", "Kernel Size", SettingType::Integer, 9, QVariant(), 1, 101, 1, 101, 2 } };
}
cv::Mat BlackHatFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(ksize, ksize));
    cv::Mat out;
    cv::morphologyEx(image, out, cv::MORPH_BLACKHAT, kernel);
    return out;
}

// ---------------------------------------------------------------------------
// Cross & Ellipse
// ---------------------------------------------------------------------------
cv::Mat DilateCrossFilter::process(const cv::Mat& image) {
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(5, 5));
    cv::Mat out;
    cv::dilate(image, out, kernel, cv::Point(-1,-1), 1);
    return out;
}

cv::Mat ErodeCrossFilter::process(const cv::Mat& image) {
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(5, 5));
    cv::Mat out;
    cv::erode(image, out, kernel, cv::Point(-1,-1), 1);
    return out;
}

cv::Mat DilateEllipseFilter::process(const cv::Mat& image) {
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
    cv::Mat out;
    cv::dilate(image, out, kernel, cv::Point(-1,-1), 1);
    return out;
}

cv::Mat ErodeEllipseFilter::process(const cv::Mat& image) {
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
    cv::Mat out;
    cv::erode(image, out, kernel, cv::Point(-1,-1), 1);
    return out;
}
