#include "filters/threshold_extra_filters.h"
#include <opencv2/imgproc.hpp>
#include <vector>

// ---------------------------------------------------------------------------
// Threshold Filters
// ---------------------------------------------------------------------------
cv::Mat OtsuThresholdFilter::process(const cv::Mat& image) {
    cv::Mat img;
    if (image.channels() == 3) cv::cvtColor(image, img, cv::COLOR_BGR2GRAY);
    else img = image;
    
    cv::Mat out;
    cv::threshold(img, out, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
    return out;
}

QVector<PFMSetting> AdaptiveThresholdFilter::defineSettings() const {
    return {
        { "block_size", "Block Size", SettingType::Integer, 11, QVariant(), 3, 99, 3, 99, 2 },
        { "c", "C (Constant)", SettingType::Number, 2.0, QVariant(), -10.0, 10.0, -10.0, 10.0, 0.5 }
    };
}
cv::Mat AdaptiveThresholdFilter::process(const cv::Mat& image) {
    cv::Mat img;
    if (image.channels() == 3) cv::cvtColor(image, img, cv::COLOR_BGR2GRAY);
    else img = image;
    
    int block = get("block_size").toInt();
    if (block % 2 == 0) block += 1;
    double c = get("c").toDouble();
    
    cv::Mat out;
    cv::adaptiveThreshold(img, out, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, block, c);
    return out;
}

QVector<PFMSetting> TruncateThresholdFilter::defineSettings() const {
    return {
        { "threshold", "Threshold", SettingType::Integer, 127, QVariant(), 0, 255, 0, 255, 1 }
    };
}
cv::Mat TruncateThresholdFilter::process(const cv::Mat& image) {
    int thresh = get("threshold").toInt();
    cv::Mat out;
    cv::threshold(image, out, thresh, 255, cv::THRESH_TRUNC);
    return out;
}

QVector<PFMSetting> ToZeroThresholdFilter::defineSettings() const {
    return {
        { "threshold", "Threshold", SettingType::Integer, 127, QVariant(), 0, 255, 0, 255, 1 }
    };
}
cv::Mat ToZeroThresholdFilter::process(const cv::Mat& image) {
    int thresh = get("threshold").toInt();
    cv::Mat out;
    cv::threshold(image, out, thresh, 255, cv::THRESH_TOZERO);
    return out;
}

// ---------------------------------------------------------------------------
// Extra/Color Filters
// ---------------------------------------------------------------------------
cv::Mat EqualizeHistFilter::process(const cv::Mat& image) {
    cv::Mat out;
    if (image.channels() == 3) {
        cv::Mat yuv;
        cv::cvtColor(image, yuv, cv::COLOR_BGR2YUV);
        std::vector<cv::Mat> channels;
        cv::split(yuv, channels);
        cv::equalizeHist(channels[0], channels[0]);
        cv::merge(channels, yuv);
        cv::cvtColor(yuv, out, cv::COLOR_YUV2BGR);
    } else {
        cv::equalizeHist(image, out);
    }
    return out;
}

cv::Mat AutoContrastFilter::process(const cv::Mat& image) {
    double minVal, maxVal;
    // Calculate global min max across all channels (or simply use normalize)
    cv::Mat out;
    cv::normalize(image, out, 0, 255, cv::NORM_MINMAX);
    return out;
}

cv::Mat AutoColorFilter::process(const cv::Mat& image) {
    cv::Mat out;
    if (image.channels() == 3) {
        std::vector<cv::Mat> channels;
        cv::split(image, channels);
        for (int i = 0; i < 3; ++i) {
            cv::normalize(channels[i], channels[i], 0, 255, cv::NORM_MINMAX);
        }
        cv::merge(channels, out);
    } else {
        cv::normalize(image, out, 0, 255, cv::NORM_MINMAX);
    }
    return out;
}

cv::Mat ColorizeFilter::process(const cv::Mat& image) {
    cv::Mat bgr;
    if (image.channels() == 1) {
        cv::cvtColor(image, bgr, cv::COLOR_GRAY2BGR);
    } else {
        bgr = image;
    }
    
    cv::Mat hsv;
    cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);
    
    std::vector<cv::Mat> channels;
    cv::split(hsv, channels);
    channels[0].setTo(15); // OpenCV hue is 0-179, so 30/2 = 15
    channels[1] += 50; // Add saturation
    cv::merge(channels, hsv);
    
    cv::Mat out;
    cv::cvtColor(hsv, out, cv::COLOR_HSV2BGR);
    return out;
}

cv::Mat InvertHueFilter::process(const cv::Mat& image) {
    if (image.channels() != 3) return image.clone();
    
    cv::Mat hsv;
    cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);
    
    std::vector<cv::Mat> channels;
    cv::split(hsv, channels);
    
    cv::Mat h16;
    channels[0].convertTo(h16, CV_16S);
    h16 += 90;
    
    // In opencv modulo
    for (int r = 0; r < h16.rows; ++r) {
        short* ptr = h16.ptr<short>(r);
        for (int c = 0; c < h16.cols; ++c) {
            ptr[c] = ptr[c] % 180;
        }
    }
    
    h16.convertTo(channels[0], CV_8U);
    cv::merge(channels, hsv);
    
    cv::Mat out;
    cv::cvtColor(hsv, out, cv::COLOR_HSV2BGR);
    return out;
}
