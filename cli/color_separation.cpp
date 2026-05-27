#include "color_separation.h"
#include <opencv2/imgproc.hpp>
#include <algorithm>

std::vector<cv::Mat> splitGrayscale(const cv::Mat& bgrImage) {
    if (bgrImage.channels() == 1) {
        return {bgrImage.clone()};
    }
    cv::Mat gray;
    if (bgrImage.channels() == 4) {
        cv::cvtColor(bgrImage, gray, cv::COLOR_BGRA2GRAY);
    } else {
        cv::cvtColor(bgrImage, gray, cv::COLOR_BGR2GRAY);
    }
    return {gray};
}

std::vector<cv::Mat> splitRGB(const cv::Mat& bgrImage) {
    if (bgrImage.channels() == 1) {
        return {bgrImage.clone(), bgrImage.clone(), bgrImage.clone()};
    }
    
    cv::Mat bgr;
    if (bgrImage.channels() == 4) {
        cv::cvtColor(bgrImage, bgr, cv::COLOR_BGRA2BGR);
    } else {
        bgr = bgrImage;
    }
    
    std::vector<cv::Mat> channels;
    cv::split(bgr, channels);
    
    cv::Mat rDensity, gDensity, bDensity;
    cv::bitwise_not(channels[2], rDensity);
    cv::bitwise_not(channels[1], gDensity);
    cv::bitwise_not(channels[0], bDensity);
    
    return {rDensity, gDensity, bDensity};
}

std::vector<cv::Mat> splitCMYK(const cv::Mat& bgrImage) {
    if (bgrImage.channels() == 1) {
        return {bgrImage.clone(), bgrImage.clone(), bgrImage.clone(), bgrImage.clone()};
    }
    
    cv::Mat bgr;
    if (bgrImage.channels() == 4) {
        cv::cvtColor(bgrImage, bgr, cv::COLOR_BGRA2BGR);
    } else {
        bgr = bgrImage;
    }
    
    int rows = bgr.rows;
    int cols = bgr.cols;
    
    cv::Mat cDensity(rows, cols, CV_8UC1);
    cv::Mat mDensity(rows, cols, CV_8UC1);
    cv::Mat yDensity(rows, cols, CV_8UC1);
    cv::Mat kDensity(rows, cols, CV_8UC1);
    
    for (int r = 0; r < rows; ++r) {
        const uchar* ptr = bgr.ptr<uchar>(r);
        uchar* cPtr = cDensity.ptr<uchar>(r);
        uchar* mPtr = mDensity.ptr<uchar>(r);
        uchar* yPtr = yDensity.ptr<uchar>(r);
        uchar* kPtr = kDensity.ptr<uchar>(r);
        
        for (int c = 0; c < cols; ++c) {
            float blue = ptr[c * 3 + 0] / 255.0f;
            float green = ptr[c * 3 + 1] / 255.0f;
            float red = ptr[c * 3 + 2] / 255.0f;
            
            float k = 1.0f - std::max({red, green, blue});
            float cyan = 0.0f;
            float magenta = 0.0f;
            float yellow = 0.0f;
            
            if (k < 1.0f) {
                cyan = (1.0f - red - k) / (1.0f - k);
                magenta = (1.0f - green - k) / (1.0f - k);
                yellow = (1.0f - blue - k) / (1.0f - k);
            }
            
            cPtr[c] = static_cast<uchar>(cyan * 255.0f);
            mPtr[c] = static_cast<uchar>(magenta * 255.0f);
            yPtr[c] = static_cast<uchar>(yellow * 255.0f);
            kPtr[c] = static_cast<uchar>(k * 255.0f);
        }
    }
    
    return {cDensity, mDensity, yDensity, kDensity};
}
