#include "filters/basic_filters.h"
#include <opencv2/imgproc.hpp>

// ---------------------------------------------------------------------------
// BrightnessFilter
// ---------------------------------------------------------------------------
QVector<PFMSetting> BrightnessFilter::defineSettings() const {
    return {
        { "brightness", "Brightness", SettingType::Number, 0.0, QVariant(), -255.0, 255.0, -255.0, 255.0, 1.0 }
    };
}

cv::Mat BrightnessFilter::process(const cv::Mat& image) {
    double b = get("brightness").toDouble();
    if (b == 0.0) return image.clone();

    cv::Mat img;
    image.convertTo(img, CV_32F);
    img += b;
    
    cv::Mat out;
    img.convertTo(out, CV_8U, 1.0, 0.0); // automatically clips 0-255
    return out;
}

// ---------------------------------------------------------------------------
// ContrastFilter
// ---------------------------------------------------------------------------
QVector<PFMSetting> ContrastFilter::defineSettings() const {
    return {
        { "contrast", "Contrast", SettingType::Number, 1.0, QVariant(), 0.0, 5.0, 0.0, 5.0, 0.1 }
    };
}

cv::Mat ContrastFilter::process(const cv::Mat& image) {
    double c = get("contrast").toDouble();
    if (std::abs(c - 1.0) < 1e-4) return image.clone();

    cv::Mat img;
    image.convertTo(img, CV_32F);
    img = (img - 127.5) * c + 127.5;

    cv::Mat out;
    img.convertTo(out, CV_8U);
    return out;
}

// ---------------------------------------------------------------------------
// InvertFilter
// ---------------------------------------------------------------------------
cv::Mat InvertFilter::process(const cv::Mat& image) {
    cv::Mat out;
    cv::bitwise_not(image, out);
    return out;
}

// ---------------------------------------------------------------------------
// ThresholdFilter
// ---------------------------------------------------------------------------
QVector<PFMSetting> ThresholdFilter::defineSettings() const {
    return {
        { "threshold", "Threshold", SettingType::Integer, 127, QVariant(), 0, 255, 0, 255, 1 }
    };
}

cv::Mat ThresholdFilter::process(const cv::Mat& image) {
    int t = get("threshold").toInt();
    cv::Mat out;
    cv::threshold(image, out, t, 255, cv::THRESH_BINARY);
    return out;
}

// ---------------------------------------------------------------------------
// UnsharpMaskFilter
// ---------------------------------------------------------------------------
QVector<PFMSetting> UnsharpMaskFilter::defineSettings() const {
    return {
        { "radius", "Radius", SettingType::Integer, 5, QVariant(), 1, 50, 1, 50, 1 },
        { "amount", "Amount", SettingType::Number,  1.5, QVariant(), 0.0, 5.0, 0.0, 5.0, 0.1 }
    };
}

cv::Mat UnsharpMaskFilter::process(const cv::Mat& image) {
    int r = get("radius").toInt();
    double a = get("amount").toDouble();

    int ksize = r * 2 + 1;
    cv::Mat blurred;
    cv::GaussianBlur(image, blurred, cv::Size(ksize, ksize), 0);

    cv::Mat imgFloat, blurredFloat;
    image.convertTo(imgFloat, CV_32F);
    blurred.convertTo(blurredFloat, CV_32F);

    cv::Mat sharpened = imgFloat + (imgFloat - blurredFloat) * a;
    
    cv::Mat out;
    sharpened.convertTo(out, CV_8U);
    return out;
}
