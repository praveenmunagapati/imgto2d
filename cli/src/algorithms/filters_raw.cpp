#include "filters_raw.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <random>

// --- base_filter.cpp ---

// --- basic_filters.cpp ---
#include <opencv2/imgproc.hpp>

// ---------------------------------------------------------------------------
// BrightnessFilter
// ---------------------------------------------------------------------------
std::vector<PFMSetting> BrightnessFilter::defineSettings() const {
    return {
        { "brightness", "Brightness", SettingType::Number, 0.0, SettingValue(), -255.0, 255.0, -255.0, 255.0, 1.0 }
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
std::vector<PFMSetting> ContrastFilter::defineSettings() const {
    return {
        { "contrast", "Contrast", SettingType::Number, 1.0, SettingValue(), 0.0, 5.0, 0.0, 5.0, 0.1 }
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
std::vector<PFMSetting> InvertFilter::defineSettings() const { return {}; }
cv::Mat InvertFilter::process(const cv::Mat& image) {
    cv::Mat out;
    cv::bitwise_not(image, out);
    return out;
}

// ---------------------------------------------------------------------------
// ThresholdFilter
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// UnsharpMaskFilter
// ---------------------------------------------------------------------------
std::vector<PFMSetting> UnsharpMaskFilter::defineSettings() const {
    return {
        { "radius", "Radius", SettingType::Integer, 5, SettingValue(), 1, 50, 1, 50, 1 },
        { "amount", "Amount", SettingType::Number,  1.5, SettingValue(), 0.0, 5.0, 0.0, 5.0, 0.1 }
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

// --- color_filters.cpp ---
#include <opencv2/imgproc.hpp>
#include <cmath>

// ---------------------------------------------------------------------------
// GrayscaleFilter
// ---------------------------------------------------------------------------
std::vector<PFMSetting> GrayscaleFilter::defineSettings() const { return {}; }
cv::Mat GrayscaleFilter::process(const cv::Mat& image) {
    if (image.channels() == 3) {
        cv::Mat out;
        cv::cvtColor(image, out, cv::COLOR_BGR2GRAY);
        return out;
    } else if (image.channels() == 4) {
        cv::Mat out;
        cv::cvtColor(image, out, cv::COLOR_BGRA2GRAY);
        return out;
    }
    return image.clone();
}

std::vector<PFMSetting> DesaturateFilter::defineSettings() const { return {}; }
cv::Mat DesaturateFilter::process(const cv::Mat& image) {
    if (image.channels() == 3) {
        cv::Mat gray;
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
        cv::Mat out;
        cv::cvtColor(gray, out, cv::COLOR_GRAY2BGR);
        return out;
    } else if (image.channels() == 4) {
        cv::Mat gray;
        cv::cvtColor(image, gray, cv::COLOR_BGRA2GRAY);
        cv::Mat out;
        cv::cvtColor(gray, out, cv::COLOR_GRAY2BGRA);
        return out;
    }
    return image.clone();
}

// ---------------------------------------------------------------------------
// SaturationFilter
// ---------------------------------------------------------------------------
std::vector<PFMSetting> SaturationFilter::defineSettings() const {
    return {
        { "saturation", "Saturation", SettingType::Number, 1.0, SettingValue(), 0.0, 3.0, 0.0, 3.0, 0.1 }
    };
}

cv::Mat SaturationFilter::process(const cv::Mat& image) {
    if (image.channels() != 3) return image.clone();
    double sat = get("saturation").toDouble();
    if (std::abs(sat - 1.0) < 1e-4) return image.clone();

    cv::Mat hsv;
    cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);
    
    // Scale saturation (channel 1)
    std::vector<cv::Mat> channels;
    cv::split(hsv, channels);
    channels[1].convertTo(channels[1], CV_32F);
    channels[1] *= sat;
    channels[1].convertTo(channels[1], CV_8U);
    cv::merge(channels, hsv);

    cv::Mat out;
    cv::cvtColor(hsv, out, cv::COLOR_HSV2BGR);
    return out;
}

// ---------------------------------------------------------------------------
// HueFilter
// ---------------------------------------------------------------------------
std::vector<PFMSetting> HueFilter::defineSettings() const {
    return {
        { "hue", "Hue Shift", SettingType::Integer, 0, SettingValue(), -180, 180, -180, 180, 1 }
    };
}

cv::Mat HueFilter::process(const cv::Mat& image) {
    if (image.channels() != 3) return image.clone();
    int hue = get("hue").toInt();
    if (hue == 0) return image.clone();

    cv::Mat hsv;
    cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);
    
    // Shift Hue (channel 0)
    std::vector<cv::Mat> channels;
    cv::split(hsv, channels);
    
    channels[0].convertTo(channels[0], CV_16S);
    channels[0] += hue;
    
    // Wrap around 180
    for(int r = 0; r < channels[0].rows; r++) {
        short* row = channels[0].ptr<short>(r);
        for(int c = 0; c < channels[0].cols; c++) {
            if (row[c] < 0) row[c] += 180;
            if (row[c] >= 180) row[c] -= 180;
        }
    }
    
    channels[0].convertTo(channels[0], CV_8U);
    cv::merge(channels, hsv);

    cv::Mat out;
    cv::cvtColor(hsv, out, cv::COLOR_HSV2BGR);
    return out;
}

// ---------------------------------------------------------------------------
// GammaFilter
// ---------------------------------------------------------------------------
std::vector<PFMSetting> GammaFilter::defineSettings() const {
    return {
        { "gamma", "Gamma", SettingType::Number, 1.0, SettingValue(), 0.1, 5.0, 0.1, 5.0, 0.1 }
    };
}

cv::Mat GammaFilter::process(const cv::Mat& image) {
    double gamma = get("gamma").toDouble();
    if (std::abs(gamma - 1.0) < 1e-4) return image.clone();

    double invGamma = 1.0 / gamma;
    cv::Mat lut(1, 256, CV_8U);
    uchar* p = lut.ptr();
    for(int i = 0; i < 256; ++i) {
        p[i] = cv::saturate_cast<uchar>(std::pow(i / 255.0, invGamma) * 255.0);
    }
    
    cv::Mat out;
    cv::LUT(image, lut, out);
    return out;
}

// ---------------------------------------------------------------------------
// ExposureFilter
// ---------------------------------------------------------------------------
std::vector<PFMSetting> ExposureFilter::defineSettings() const {
    return {
        { "exposure", "Exposure (EV)", SettingType::Number, 0.0, SettingValue(), -5.0, 5.0, -5.0, 5.0, 0.1 }
    };
}

cv::Mat ExposureFilter::process(const cv::Mat& image) {
    double ev = get("exposure").toDouble();
    if (std::abs(ev) < 1e-4) return image.clone();

    double multiplier = std::pow(2.0, ev);
    cv::Mat imgFloat;
    image.convertTo(imgFloat, CV_32F);
    imgFloat *= multiplier;

    cv::Mat out;
    imgFloat.convertTo(out, CV_8U);
    return out;
}

// ---------------------------------------------------------------------------
// SepiaFilter
// ---------------------------------------------------------------------------
std::vector<PFMSetting> SepiaFilter::defineSettings() const {
    return {
        { "intensity", "Intensity", SettingType::Percentage, 100.0, SettingValue(), 0.0, 100.0, 0.0, 100.0, 1.0 }
    };
}
cv::Mat SepiaFilter::process(const cv::Mat& image) {
    cv::Mat img3;
    if (image.channels() == 1) {
        cv::cvtColor(image, img3, cv::COLOR_GRAY2BGR);
    } else {
        img3 = image.clone();
    }

    cv::Mat kernel = (cv::Mat_<float>(3, 3) <<
        0.272f, 0.534f, 0.131f,
        0.349f, 0.686f, 0.168f,
        0.393f, 0.769f, 0.189f);
    
    cv::Mat sepia;
    cv::transform(img3, sepia, kernel);
    sepia.convertTo(sepia, CV_8U);
    
    float intensity = get("intensity").toDouble() / 100.0f;
    if (intensity >= 0.999f) return sepia;
    if (intensity <= 0.001f) return img3;
    
    cv::Mat out;
    cv::addWeighted(sepia, intensity, img3, 1.0f - intensity, 0, out);
    return out;
}

// ---------------------------------------------------------------------------
// CLAHEFilter
// ---------------------------------------------------------------------------
std::vector<PFMSetting> CLAHEFilter::defineSettings() const {
    return {
        { "clip_limit", "Clip Limit", SettingType::Number, 2.0, SettingValue(), 0.1, 10.0, 0.1, 10.0, 0.1 },
        { "grid_size", "Grid Size", SettingType::Integer, 8, SettingValue(), 2, 32, 2, 32, 2 }
    };
}

cv::Mat CLAHEFilter::process(const cv::Mat& image) {
    double clip = get("clip_limit").toDouble();
    int grid = get("grid_size").toInt();

    auto clahe = cv::createCLAHE(clip, cv::Size(grid, grid));

    if (image.channels() == 3) {
        cv::Mat lab;
        cv::cvtColor(image, lab, cv::COLOR_BGR2Lab);
        
        std::vector<cv::Mat> channels;
        cv::split(lab, channels);
        clahe->apply(channels[0], channels[0]); // Apply to L channel
        cv::merge(channels, lab);
        
        cv::Mat out;
        cv::cvtColor(lab, out, cv::COLOR_Lab2BGR);
        return out;
    } else if (image.channels() == 1) {
        cv::Mat out;
        clahe->apply(image, out);
        return out;
    }
    return image.clone();
}

// ---------------------------------------------------------------------------
// TemperatureFilter
// ---------------------------------------------------------------------------
std::vector<PFMSetting> TemperatureFilter::defineSettings() const {
    return {
        { "temperature", "Temp", SettingType::Number, 0.0, SettingValue(), -100.0, 100.0, -100.0, 100.0, 1.0 }
    };
}

cv::Mat TemperatureFilter::process(const cv::Mat& image) {
    if (image.channels() != 3) return image.clone();
    double temp = get("temperature").toDouble();
    if (std::abs(temp) < 1e-4) return image.clone();

    cv::Mat img16;
    image.convertTo(img16, CV_16S);
    
    // Simple heuristic: OpenCV is BGR
    // B = 0, G = 1, R = 2
    // Positive temp adds red, removes blue
    std::vector<cv::Mat> channels;
    cv::split(img16, channels);
    channels[2] += temp; // Red
    channels[0] -= temp; // Blue
    cv::merge(channels, img16);

    cv::Mat out;
    img16.convertTo(out, CV_8U);
    return out;
}

// --- edge_blur_filters.cpp ---
#include <opencv2/imgproc.hpp>
#include <cmath>

// ---------------------------------------------------------------------------
// Edge Filters
// ---------------------------------------------------------------------------
std::vector<PFMSetting> CannyFilter::defineSettings() const {
    return {
        { "threshold1", "Threshold 1", SettingType::Integer, 100, SettingValue(), 0, 255, 0, 255, 1 },
        { "threshold2", "Threshold 2", SettingType::Integer, 200, SettingValue(), 0, 255, 0, 255, 1 }
    };
}

cv::Mat CannyFilter::process(const cv::Mat& image) {
    int t1 = get("threshold1").toInt();
    int t2 = get("threshold2").toInt();

    cv::Mat gray;
    if (image.channels() == 3) cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    else if (image.channels() == 4) cv::cvtColor(image, gray, cv::COLOR_BGRA2GRAY);
    else gray = image;

    cv::Mat out;
    cv::Canny(gray, out, t1, t2);
    // Return a 3-channel image if input was 3-channel
    if (image.channels() == 3) {
        cv::cvtColor(out, out, cv::COLOR_GRAY2BGR);
    }
    return out;
}


std::vector<PFMSetting> SobelFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 3, SettingValue(), 1, 7, 1, 7, 2 },
        { "scale", "Scale", SettingType::Number, 1.0, SettingValue(), 0.1, 10.0, 0.1, 10.0, 0.1 }
    };
}

cv::Mat SobelFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    if (ksize % 2 == 0) ksize += 1;
    double scale = get("scale").toDouble();

    cv::Mat gray;
    if (image.channels() == 3) cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    else if (image.channels() == 4) cv::cvtColor(image, gray, cv::COLOR_BGRA2GRAY);
    else gray = image;

    cv::Mat sobelx, sobely;
    cv::Sobel(gray, sobelx, CV_64F, 1, 0, ksize, scale);
    cv::Sobel(gray, sobely, CV_64F, 0, 1, ksize, scale);

    cv::Mat mag;
    cv::magnitude(sobelx, sobely, mag);
    
    cv::Mat out;
    mag.convertTo(out, CV_8U);
    if (image.channels() == 3) {
        cv::cvtColor(out, out, cv::COLOR_GRAY2BGR);
    }
    return out;
}


std::vector<PFMSetting> LaplacianFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 3, SettingValue(), 1, 7, 1, 7, 2 }
    };
}

cv::Mat LaplacianFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    if (ksize % 2 == 0) ksize += 1;

    cv::Mat gray;
    if (image.channels() == 3) cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    else if (image.channels() == 4) cv::cvtColor(image, gray, cv::COLOR_BGRA2GRAY);
    else gray = image;

    cv::Mat laplacian;
    cv::Laplacian(gray, laplacian, CV_64F, ksize);
    laplacian = cv::abs(laplacian);

    cv::Mat out;
    laplacian.convertTo(out, CV_8U);
    if (image.channels() == 3) {
        cv::cvtColor(out, out, cv::COLOR_GRAY2BGR);
    }
    return out;
}

std::vector<PFMSetting> PrewittFilter::defineSettings() const { return {}; }
cv::Mat PrewittFilter::process(const cv::Mat& image) {
    cv::Mat kernelx = (cv::Mat_<float>(3, 3) << 1, 1, 1, 0, 0, 0, -1, -1, -1);
    cv::Mat kernely = (cv::Mat_<float>(3, 3) << -1, 0, 1, -1, 0, 1, -1, 0, 1);
    
    cv::Mat imgx, imgy;
    cv::filter2D(image, imgx, CV_32F, kernelx);
    cv::filter2D(image, imgy, CV_32F, kernely);
    
    cv::Mat mag;
    cv::magnitude(imgx, imgy, mag);
    
    cv::Mat out;
    mag.convertTo(out, CV_8U);
    return out;
}

std::vector<PFMSetting> ScharrFilter::defineSettings() const { return {}; }
cv::Mat ScharrFilter::process(const cv::Mat& image) {
    cv::Mat scharrx, scharry;
    cv::Scharr(image, scharrx, CV_32F, 1, 0);
    cv::Scharr(image, scharry, CV_32F, 0, 1);
    
    cv::Mat mag;
    cv::magnitude(scharrx, scharry, mag);
    
    cv::Mat out;
    mag.convertTo(out, CV_8U);
    return out;
}

std::vector<PFMSetting> DoGFilter::defineSettings() const {
    return {
        { "sigma1", "Sigma 1", SettingType::Number, 1.0, SettingValue(), 0.1, 10.0, 0.1, 10.0, 0.1 },
        { "sigma2", "Sigma 2", SettingType::Number, 2.0, SettingValue(), 0.1, 10.0, 0.1, 10.0, 0.1 }
    };
}

cv::Mat DoGFilter::process(const cv::Mat& image) {
    double s1 = get("sigma1").toDouble();
    double s2 = get("sigma2").toDouble();
    
    cv::Mat blur1, blur2;
    cv::GaussianBlur(image, blur1, cv::Size(0,0), s1);
    cv::GaussianBlur(image, blur2, cv::Size(0,0), s2);
    
    cv::Mat dog;
    cv::absdiff(blur1, blur2, dog);
    dog *= 2;
    
    return dog;
}

std::vector<PFMSetting> RidgeDetectionFilter::defineSettings() const { return {}; }
cv::Mat RidgeDetectionFilter::process(const cv::Mat& image) {
    cv::Mat img;
    if (image.channels() == 3) cv::cvtColor(image, img, cv::COLOR_BGR2GRAY);
    else img = image;

    cv::Mat dx, dy, dxy;
    cv::Sobel(img, dx, CV_32F, 2, 0);
    cv::Sobel(img, dy, CV_32F, 0, 2);
    cv::Sobel(img, dxy, CV_32F, 1, 1);
    
    // Trace and Det
    cv::Mat trace = dx + dy;
    cv::Mat det = dx.mul(dy) - dxy.mul(dxy);
    
    // lambda1
    cv::Mat discriminant;
    cv::sqrt(cv::max(trace.mul(trace) - 4*det, 0), discriminant);
    cv::Mat lambda1 = 0.5 * (trace + discriminant);
    lambda1 = cv::abs(lambda1);
    
    double minVal, maxVal;
    cv::minMaxLoc(lambda1, &minVal, &maxVal);
    if (maxVal > 0) {
        lambda1 = (lambda1 / maxVal) * 255.0;
    }
    
    cv::Mat out;
    lambda1.convertTo(out, CV_8U);
    if (image.channels() == 3) {
        cv::cvtColor(out, out, cv::COLOR_GRAY2BGR);
    }
    return out;
}

std::vector<PFMSetting> HighPassFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 11, SettingValue(), 3, 31, 3, 31, 2 }
    };
}
cv::Mat HighPassFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    if (ksize % 2 == 0) ksize++;
    cv::Mat blur;
    cv::GaussianBlur(image, blur, cv::Size(ksize, ksize), 0);
    
    cv::Mat img32, blur32;
    image.convertTo(img32, CV_32F);
    blur.convertTo(blur32, CV_32F);
    
    cv::Mat hp = img32 - blur32 + 127.0;
    
    cv::Mat out;
    hp.convertTo(out, CV_8U);
    return out;
}


// ---------------------------------------------------------------------------
// Blur Filters
// ---------------------------------------------------------------------------
std::vector<PFMSetting> GaussianBlurFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 5, SettingValue(), 1, 51, 1, 51, 2 },
        { "sigma", "Sigma", SettingType::Number, 0.0, SettingValue(), 0.0, 10.0, 0.0, 10.0, 0.5 }
    };
}

cv::Mat GaussianBlurFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    if (ksize % 2 == 0) ksize += 1;
    double sigma = get("sigma").toDouble();

    cv::Mat out;
    cv::GaussianBlur(image, out, cv::Size(ksize, ksize), sigma);
    return out;
}


std::vector<PFMSetting> MedianBlurFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 5, SettingValue(), 3, 51, 3, 51, 2 }
    };
}

cv::Mat MedianBlurFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    if (ksize % 2 == 0) ksize += 1;

    cv::Mat out;
    cv::medianBlur(image, out, ksize);
    return out;
}


std::vector<PFMSetting> BoxBlurFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 5, SettingValue(), 1, 51, 1, 51, 1 }
    };
}

cv::Mat BoxBlurFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();

    cv::Mat out;
    cv::blur(image, out, cv::Size(ksize, ksize));
    return out;
}

std::vector<PFMSetting> MotionBlurFilter::defineSettings() const {
    return {
        { "size", "Size", SettingType::Integer, 15, SettingValue(), 3, 101, 3, 101, 2 }
    };
}

cv::Mat MotionBlurFilter::process(const cv::Mat& image) {
    int size = get("size").toInt();
    if (size % 2 == 0) size += 1;
    
    cv::Mat kernel = cv::Mat::zeros(size, size, CV_32F);
    int mid = size / 2;
    for (int i = 0; i < size; ++i) {
        kernel.at<float>(mid, i) = 1.0f / size;
    }
    
    cv::Mat out;
    cv::filter2D(image, out, -1, kernel);
    return out;
}

std::vector<PFMSetting> BilateralFilter::defineSettings() const {
    return {
        { "d", "Diameter", SettingType::Integer, 9, SettingValue(), 1, 31, 1, 31, 2 },
        { "sigma_color", "Sigma Color", SettingType::Number, 75.0, SettingValue(), 1.0, 200.0, 1.0, 200.0, 1.0 },
        { "sigma_space", "Sigma Space", SettingType::Number, 75.0, SettingValue(), 1.0, 200.0, 1.0, 200.0, 1.0 }
    };
}
cv::Mat BilateralFilter::process(const cv::Mat& image) {
    int d = get("d").toInt();
    double sc = get("sigma_color").toDouble();
    double ss = get("sigma_space").toDouble();
    cv::Mat out;
    cv::bilateralFilter(image, out, d, sc, ss);
    return out;
}

std::vector<PFMSetting> LowPassFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 21, SettingValue(), 3, 99, 3, 99, 2 }
    };
}
cv::Mat LowPassFilter::process(const cv::Mat& image) {
    int k = get("ksize").toInt(); if (k % 2 == 0) k++;
    cv::Mat out;
    cv::GaussianBlur(image, out, cv::Size(k, k), 0);
    return out;
}

std::vector<PFMSetting> SharpenMoreFilter::defineSettings() const {
    return {
        { "amount", "Amount", SettingType::Number, 1.0, SettingValue(), 0.1, 5.0, 0.1, 5.0, 0.1 }
    };
}
cv::Mat SharpenMoreFilter::process(const cv::Mat& image) {
    float a = get("amount").toDouble();
    cv::Mat kernel = (cv::Mat_<float>(3,3) << -a, -a, -a, -a, 1+8*a, -a, -a, -a, -a);
    cv::Mat out;
    cv::filter2D(image, out, -1, kernel);
    return out;
}


// --- morph_filters.cpp ---
#include <opencv2/imgproc.hpp>

// ---------------------------------------------------------------------------
// Dilation & Erosion
// ---------------------------------------------------------------------------
std::vector<PFMSetting> DilationFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 3, SettingValue(), 1, 31, 1, 31, 2 },
        { "iterations", "Iterations", SettingType::Integer, 1, SettingValue(), 1, 10, 1, 10, 1 }
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

std::vector<PFMSetting> ErosionFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 3, SettingValue(), 1, 31, 1, 31, 2 },
        { "iterations", "Iterations", SettingType::Integer, 1, SettingValue(), 1, 10, 1, 10, 1 }
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
std::vector<PFMSetting> OpeningFilter::defineSettings() const {
    return { { "ksize", "Kernel Size", SettingType::Integer, 3, SettingValue(), 1, 31, 1, 31, 2 } };
}
cv::Mat OpeningFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(ksize, ksize));
    cv::Mat out;
    cv::morphologyEx(image, out, cv::MORPH_OPEN, kernel);
    return out;
}

std::vector<PFMSetting> ClosingFilter::defineSettings() const {
    return { { "ksize", "Kernel Size", SettingType::Integer, 3, SettingValue(), 1, 31, 1, 31, 2 } };
}
cv::Mat ClosingFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(ksize, ksize));
    cv::Mat out;
    cv::morphologyEx(image, out, cv::MORPH_CLOSE, kernel);
    return out;
}

std::vector<PFMSetting> MorphGradientFilter::defineSettings() const {
    return { { "ksize", "Kernel Size", SettingType::Integer, 3, SettingValue(), 1, 31, 1, 31, 2 } };
}
cv::Mat MorphGradientFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(ksize, ksize));
    cv::Mat out;
    cv::morphologyEx(image, out, cv::MORPH_GRADIENT, kernel);
    return out;
}

std::vector<PFMSetting> TopHatFilter::defineSettings() const {
    return { { "ksize", "Kernel Size", SettingType::Integer, 9, SettingValue(), 1, 101, 1, 101, 2 } };
}
cv::Mat TopHatFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(ksize, ksize));
    cv::Mat out;
    cv::morphologyEx(image, out, cv::MORPH_TOPHAT, kernel);
    return out;
}

std::vector<PFMSetting> BlackHatFilter::defineSettings() const {
    return { { "ksize", "Kernel Size", SettingType::Integer, 9, SettingValue(), 1, 101, 1, 101, 2 } };
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
std::vector<PFMSetting> DilateCrossFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 5, SettingValue(), 3, 31, 3, 31, 2 },
        { "iterations", "Iterations", SettingType::Integer, 1, SettingValue(), 1, 10, 1, 10, 1 }
    };
}
cv::Mat DilateCrossFilter::process(const cv::Mat& image) {
    int k = get("ksize").toInt(); int iters = get("iterations").toInt();
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(k, k));
    cv::Mat out;
    cv::dilate(image, out, kernel, cv::Point(-1,-1), iters);
    return out;
}

std::vector<PFMSetting> ErodeCrossFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 5, SettingValue(), 3, 31, 3, 31, 2 },
        { "iterations", "Iterations", SettingType::Integer, 1, SettingValue(), 1, 10, 1, 10, 1 }
    };
}
cv::Mat ErodeCrossFilter::process(const cv::Mat& image) {
    int k = get("ksize").toInt(); int iters = get("iterations").toInt();
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(k, k));
    cv::Mat out;
    cv::erode(image, out, kernel, cv::Point(-1,-1), iters);
    return out;
}

std::vector<PFMSetting> DilateEllipseFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 5, SettingValue(), 3, 31, 3, 31, 2 },
        { "iterations", "Iterations", SettingType::Integer, 1, SettingValue(), 1, 10, 1, 10, 1 }
    };
}
cv::Mat DilateEllipseFilter::process(const cv::Mat& image) {
    int k = get("ksize").toInt(); int iters = get("iterations").toInt();
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(k, k));
    cv::Mat out;
    cv::dilate(image, out, kernel, cv::Point(-1,-1), iters);
    return out;
}

std::vector<PFMSetting> ErodeEllipseFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 5, SettingValue(), 3, 31, 3, 31, 2 },
        { "iterations", "Iterations", SettingType::Integer, 1, SettingValue(), 1, 10, 1, 10, 1 }
    };
}
cv::Mat ErodeEllipseFilter::process(const cv::Mat& image) {
    int k = get("ksize").toInt(); int iters = get("iterations").toInt();
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(k, k));
    cv::Mat out;
    cv::erode(image, out, kernel, cv::Point(-1,-1), iters);
    return out;
}

// --- noise_filters.cpp ---
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>

// ---------------------------------------------------------------------------
// GaussianNoiseFilter
// ---------------------------------------------------------------------------
std::vector<PFMSetting> GaussianNoiseFilter::defineSettings() const {
    return {
        { "mean", "Mean", SettingType::Number, 0.0, SettingValue(), -100.0, 100.0, -100.0, 100.0, 1.0 },
        { "sigma", "Std Dev", SettingType::Number, 25.0, SettingValue(), 0.0, 100.0, 0.0, 100.0, 1.0 }
    };
}

cv::Mat GaussianNoiseFilter::process(const cv::Mat& image) {
    double mean = get("mean").toDouble();
    double sigma = get("sigma").toDouble();

    cv::Mat noise(image.size(), image.type());
    cv::randn(noise, mean, sigma);
    
    cv::Mat img16;
    image.convertTo(img16, CV_16S);
    
    cv::Mat noise16;
    noise.convertTo(noise16, CV_16S);
    
    img16 += noise16;
    
    cv::Mat out;
    img16.convertTo(out, CV_8U);
    return out;
}

// ---------------------------------------------------------------------------
// SaltAndPepperFilter
// ---------------------------------------------------------------------------
std::vector<PFMSetting> SaltAndPepperFilter::defineSettings() const {
    return {
        { "prob", "Probability", SettingType::Percentage, 5.0, SettingValue(), 0.0, 100.0, 0.0, 100.0, 1.0 }
    };
}

cv::Mat SaltAndPepperFilter::process(const cv::Mat& image) {
    double prob = get("prob").toDouble() / 100.0;
    if (prob <= 0.0) return image.clone();

    cv::Mat out = image.clone();
    cv::Mat noise(image.size(), CV_32F);
    cv::randu(noise, 0.0, 1.0);

    // Apply pepper
    out.setTo(cv::Scalar::all(0), noise < (prob / 2.0));
    // Apply salt
    out.setTo(cv::Scalar::all(255), (noise >= (prob / 2.0)) & (noise < prob));
    
    return out;
}

// ---------------------------------------------------------------------------
// DenoiseFilter
// ---------------------------------------------------------------------------
std::vector<PFMSetting> DenoiseFilter::defineSettings() const {
    return {
        { "h", "Strength (h)", SettingType::Number, 10.0, SettingValue(), 1.0, 50.0, 1.0, 50.0, 1.0 }
    };
}

cv::Mat DenoiseFilter::process(const cv::Mat& image) {
    double h = get("h").toDouble();
    cv::Mat out;
    if (image.channels() == 3) {
        cv::fastNlMeansDenoisingColored(image, out, h, h, 7, 21);
    } else if (image.channels() == 1) {
        cv::fastNlMeansDenoising(image, out, h, 7, 21);
    } else {
        out = image.clone();
    }
    return out;
}

// ---------------------------------------------------------------------------
// GaussianNoise2Filter
// ---------------------------------------------------------------------------
std::vector<PFMSetting> GaussianNoise2Filter::defineSettings() const {
    return {
        { "sigma", "Sigma", SettingType::Number, 10.0, SettingValue(), 1.0, 100.0, 1.0, 100.0, 1.0 }
    };
}
cv::Mat GaussianNoise2Filter::process(const cv::Mat& image) {
    double sigma = get("sigma").toDouble();
    cv::Mat noise(image.size(), image.type());
    cv::randn(noise, 0.0, sigma);
    
    cv::Mat img16;
    image.convertTo(img16, CV_16S);
    
    cv::Mat noise16;
    noise.convertTo(noise16, CV_16S);
    
    img16 += noise16;
    
    cv::Mat out;
    img16.convertTo(out, CV_8U);
    return out;
}

// ---------------------------------------------------------------------------
// SpeckleNoiseFilter
// ---------------------------------------------------------------------------
std::vector<PFMSetting> SpeckleNoiseFilter::defineSettings() const {
    return {
        { "intensity", "Intensity", SettingType::Number, 0.1, SettingValue(), 0.01, 1.0, 0.01, 1.0, 0.01 }
    };
}
cv::Mat SpeckleNoiseFilter::process(const cv::Mat& image) {
    float intensity = get("intensity").toDouble();
    cv::Mat noise(image.size(), CV_32F);
    cv::randn(noise, 0.0, 1.0);
    
    cv::Mat img32;
    image.convertTo(img32, CV_32F);
    
    cv::Mat out = img32 + img32.mul(noise) * intensity;
    out.convertTo(out, CV_8U);
    return out;
}

// --- threshold_extra_filters.cpp ---
#include <opencv2/imgproc.hpp>
#include <vector>

// ---------------------------------------------------------------------------
// Threshold Filters
// ---------------------------------------------------------------------------
std::vector<PFMSetting> OtsuThresholdFilter::defineSettings() const { return {}; }
cv::Mat OtsuThresholdFilter::process(const cv::Mat& image) {
    cv::Mat img;
    if (image.channels() == 3) cv::cvtColor(image, img, cv::COLOR_BGR2GRAY);
    else img = image;
    
    cv::Mat out;
    cv::threshold(img, out, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
    return out;
}

std::vector<PFMSetting> AdaptiveThresholdFilter::defineSettings() const {
    return {
        { "block_size", "Block Size", SettingType::Integer, 11, SettingValue(), 3, 99, 3, 99, 2 },
        { "c", "C (Constant)", SettingType::Number, 2.0, SettingValue(), -10.0, 10.0, -10.0, 10.0, 0.5 }
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

std::vector<PFMSetting> TruncateThresholdFilter::defineSettings() const {
    return {
        { "threshold", "Threshold", SettingType::Integer, 127, SettingValue(), 0, 255, 0, 255, 1 }
    };
}
cv::Mat TruncateThresholdFilter::process(const cv::Mat& image) {
    int thresh = get("threshold").toInt();
    cv::Mat out;
    cv::threshold(image, out, thresh, 255, cv::THRESH_TRUNC);
    return out;
}

std::vector<PFMSetting> ToZeroThresholdFilter::defineSettings() const {
    return {
        { "threshold", "Threshold", SettingType::Integer, 127, SettingValue(), 0, 255, 0, 255, 1 }
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
std::vector<PFMSetting> EqualizeHistFilter::defineSettings() const { return {}; }
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

std::vector<PFMSetting> AutoContrastFilter::defineSettings() const { return {}; }
cv::Mat AutoContrastFilter::process(const cv::Mat& image) {
    double minVal, maxVal;
    // Calculate global min max across all channels (or simply use normalize)
    cv::Mat out;
    cv::normalize(image, out, 0, 255, cv::NORM_MINMAX);
    return out;
}

std::vector<PFMSetting> AutoColorFilter::defineSettings() const { return {}; }
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

std::vector<PFMSetting> ColorizeFilter::defineSettings() const {
    return {
        { "hue", "Hue", SettingType::Integer, 30, SettingValue(), 0, 360, 0, 360, 1 },
        { "saturation_boost", "Saturation Boost", SettingType::Integer, 50, SettingValue(), 0, 255, 0, 255, 1 }
    };
}
cv::Mat ColorizeFilter::process(const cv::Mat& image) {
    int hue = get("hue").toInt() / 2; // OpenCV hue is 0-179
    int sat_boost = get("saturation_boost").toInt();
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
    channels[0].setTo(hue);
    channels[1] += sat_boost;
    cv::merge(channels, hsv);
    
    cv::Mat out;
    cv::cvtColor(hsv, out, cv::COLOR_HSV2BGR);
    return out;
}

std::vector<PFMSetting> InvertHueFilter::defineSettings() const { return {}; }
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

// --- artistic_distort_filters.cpp ---
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>
#include <algorithm>
#include <cmath>

// ---------------------------------------------------------------------------
// Artistic
// ---------------------------------------------------------------------------
std::vector<PFMSetting> PosterizeFilter::defineSettings() const {
    return {
        { "levels", "Levels", SettingType::Integer, 4, SettingValue(), 2, 256, 2, 256, 1 }
    };
}
cv::Mat PosterizeFilter::process(const cv::Mat& image) {
    int levels = get("levels").toInt();
    if (levels >= 256) return image.clone();

    double factor1 = 256.0 / levels;
    double factor2 = 255.0 / (levels - 1);
    
    cv::Mat img32;
    image.convertTo(img32, CV_32F);
    
    cv::Mat out;
    img32 = (img32 / factor1);
    
    // Equivalent to np.floor in C++ matrix operations
    for (int r = 0; r < img32.rows; ++r) {
        float* ptr = img32.ptr<float>(r);
        for (int c = 0; c < img32.cols * img32.channels(); ++c) {
            ptr[c] = std::floor(ptr[c]) * factor2;
        }
    }
    
    img32.convertTo(out, CV_8U);
    return out;
}

std::vector<PFMSetting> EdgePreserveFilter::defineSettings() const {
    return {
        { "sigma_s", "Sigma S", SettingType::Number, 60.0, SettingValue(), 1.0, 200.0, 1.0, 200.0, 1.0 },
        { "sigma_r", "Sigma R", SettingType::Number, 0.4, SettingValue(), 0.01, 1.0, 0.01, 1.0, 0.01 }
    };
}
cv::Mat EdgePreserveFilter::process(const cv::Mat& image) {
    float ss = get("sigma_s").toDouble();
    float sr = get("sigma_r").toDouble();
    cv::Mat out;
    if (image.channels() == 3) {
        cv::edgePreservingFilter(image, out, 1, ss, sr);
    } else {
        cv::Mat bgr;
        cv::cvtColor(image, bgr, cv::COLOR_GRAY2BGR);
        cv::Mat res;
        cv::edgePreservingFilter(bgr, res, 1, ss, sr);
        cv::cvtColor(res, out, cv::COLOR_BGR2GRAY);
    }
    return out;
}

std::vector<PFMSetting> StylizationFilter::defineSettings() const {
    return {
        { "sigma_s", "Sigma S", SettingType::Number, 60.0, SettingValue(), 1.0, 200.0, 1.0, 200.0, 1.0 },
        { "sigma_r", "Sigma R", SettingType::Number, 0.45, SettingValue(), 0.01, 1.0, 0.01, 1.0, 0.01 }
    };
}
cv::Mat StylizationFilter::process(const cv::Mat& image) {
    float ss = get("sigma_s").toDouble();
    float sr = get("sigma_r").toDouble();
    cv::Mat out;
    if (image.channels() == 3) {
        cv::stylization(image, out, ss, sr);
    } else {
        cv::Mat bgr;
        cv::cvtColor(image, bgr, cv::COLOR_GRAY2BGR);
        cv::Mat res;
        cv::stylization(bgr, res, ss, sr);
        cv::cvtColor(res, out, cv::COLOR_BGR2GRAY);
    }
    return out;
}

std::vector<PFMSetting> OilPaintingFilter::defineSettings() const {
    return {
        { "size", "Size", SettingType::Integer, 5, SettingValue(), 1, 20, 1, 20, 1 },
        { "dynRatio", "Dynamic Ratio", SettingType::Integer, 1, SettingValue(), 1, 10, 1, 10, 1 }
    };
}

cv::Mat OilPaintingFilter::process(const cv::Mat& image) {
    if (image.empty()) return {};

    bool wasGray = image.channels() == 1;
    cv::Mat bgr;
    if (wasGray) cv::cvtColor(image, bgr, cv::COLOR_GRAY2BGR);
    else if (image.channels() == 4) cv::cvtColor(image, bgr, cv::COLOR_BGRA2BGR);
    else bgr = image.clone();

    int size = std::max(1, get("size").toInt());
    if (size % 2 == 0) ++size;
    int radius = size / 2;
    int dynRatio = std::max(1, get("dynRatio").toInt());

    cv::Mat gray;
    cv::cvtColor(bgr, gray, cv::COLOR_BGR2GRAY);
    cv::Mat out(bgr.size(), bgr.type());

    for (int y = 0; y < bgr.rows; ++y) {
        for (int x = 0; x < bgr.cols; ++x) {
            std::vector<int> counts(dynRatio, 0);
            std::vector<cv::Vec3i> sums(dynRatio, cv::Vec3i(0, 0, 0));

            for (int dy = -radius; dy <= radius; ++dy) {
                int yy = std::clamp(y + dy, 0, bgr.rows - 1);
                for (int dx = -radius; dx <= radius; ++dx) {
                    int xx = std::clamp(x + dx, 0, bgr.cols - 1);
                    int bin = std::clamp((int)(gray.at<uchar>(yy, xx) * dynRatio / 256.0), 0, dynRatio - 1);
                    cv::Vec3b pix = bgr.at<cv::Vec3b>(yy, xx);
                    counts[bin]++;
                    sums[bin] += cv::Vec3i(pix[0], pix[1], pix[2]);
                }
            }

            int best = 0;
            for (int i = 1; i < dynRatio; ++i)
                if (counts[i] > counts[best]) best = i;
            int n = std::max(1, counts[best]);
            out.at<cv::Vec3b>(y, x) = cv::Vec3b(
                cv::saturate_cast<uchar>(sums[best][0] / n),
                cv::saturate_cast<uchar>(sums[best][1] / n),
                cv::saturate_cast<uchar>(sums[best][2] / n));
        }
    }

    if (wasGray) {
        cv::Mat grayOut;
        cv::cvtColor(out, grayOut, cv::COLOR_BGR2GRAY);
        return grayOut;
    }
    return out;
}

std::vector<PFMSetting> DetailEnhanceFilter::defineSettings() const {
    return {
        { "sigma_s", "Sigma S", SettingType::Number, 10.0, SettingValue(), 1.0, 200.0, 1.0, 200.0, 1.0 },
        { "sigma_r", "Sigma R", SettingType::Number, 0.15, SettingValue(), 0.01, 1.0, 0.01, 1.0, 0.01 }
    };
}

cv::Mat DetailEnhanceFilter::process(const cv::Mat& image) {
    cv::Mat out;
    float sigmaS = (float)get("sigma_s").toDouble();
    float sigmaR = (float)get("sigma_r").toDouble();
    if (image.channels() == 3) {
        cv::detailEnhance(image, out, sigmaS, sigmaR);
    } else {
        cv::Mat bgr, res;
        cv::cvtColor(image, bgr, cv::COLOR_GRAY2BGR);
        cv::detailEnhance(bgr, res, sigmaS, sigmaR);
        cv::cvtColor(res, out, cv::COLOR_BGR2GRAY);
    }
    return out;
}

std::vector<PFMSetting> PencilSketchFilter::defineSettings() const {
    return {
        { "sigma_s", "Sigma S", SettingType::Number, 60.0, SettingValue(), 1.0, 200.0, 1.0, 200.0, 1.0 },
        { "sigma_r", "Sigma R", SettingType::Number, 0.07, SettingValue(), 0.01, 1.0, 0.01, 1.0, 0.01 },
        { "shade_factor", "Shade Factor", SettingType::Number, 0.05, SettingValue(), 0.01, 0.1, 0.01, 0.1, 0.01 }
    };
}

cv::Mat PencilSketchFilter::process(const cv::Mat& image) {
    cv::Mat gray, color;
    float sigmaS = (float)get("sigma_s").toDouble();
    float sigmaR = (float)get("sigma_r").toDouble();
    float shadeFactor = (float)get("shade_factor").toDouble();
    if (image.channels() == 3) {
        cv::pencilSketch(image, gray, color, sigmaS, sigmaR, shadeFactor);
    } else {
        cv::Mat bgr;
        cv::cvtColor(image, bgr, cv::COLOR_GRAY2BGR);
        cv::pencilSketch(bgr, gray, color, sigmaS, sigmaR, shadeFactor);
    }
    return gray;
}

std::vector<PFMSetting> EmbossFilter::defineSettings() const {
    return {
        { "angle", "Angle", SettingType::Number, 135.0, SettingValue(), 0.0, 360.0, 0.0, 360.0, 45.0 }
    };
}
cv::Mat EmbossFilter::process(const cv::Mat& image) {
    float angle = get("angle").toDouble() * CV_PI / 180.0;
    float dx = std::cos(angle);
    float dy = std::sin(angle);
    cv::Mat kernel = (cv::Mat_<float>(3, 3) << 
        -dx-dy, -dy, dx-dy,
        -dx,     1,  dx,
        -dx+dy,  dy, dx+dy);
    cv::Mat out;
    cv::filter2D(image, out, -1, kernel);
    return out;
}

std::vector<PFMSetting> QuantizeFilter::defineSettings() const {
    return {
        { "step", "Quantize Step", SettingType::Integer, 32, SettingValue(), 2, 128, 2, 128, 2 }
    };
}
cv::Mat QuantizeFilter::process(const cv::Mat& image) {
    float step = get("step").toInt();
    cv::Mat img32;
    image.convertTo(img32, CV_32F);
    
    for (int r = 0; r < img32.rows; ++r) {
        float* ptr = img32.ptr<float>(r);
        for (int c = 0; c < img32.cols * img32.channels(); ++c) {
            ptr[c] = std::round(ptr[c] / step) * step;
        }
    }
    
    cv::Mat out;
    img32.convertTo(out, CV_8U);
    return out;
}

// ---------------------------------------------------------------------------
// Distort
// ---------------------------------------------------------------------------
std::vector<PFMSetting> VignetteFilter::defineSettings() const {
    return {
        { "intensity", "Intensity", SettingType::Number, 150.0, SettingValue(), 10.0, 300.0, 10.0, 300.0, 5.0 }
    };
}
cv::Mat VignetteFilter::process(const cv::Mat& image) {
    double intensity = get("intensity").toDouble();
    int h = image.rows;
    int w = image.cols;
    
    cv::Mat kernelX = cv::getGaussianKernel(w, intensity);
    cv::Mat kernelY = cv::getGaussianKernel(h, intensity);
    cv::Mat kernel = kernelY * kernelX.t();
    
    cv::Mat mask;
    cv::normalize(kernel, mask, 0.0, 1.0, cv::NORM_MINMAX);
    mask.convertTo(mask, CV_32F);
    
    if (image.channels() == 3) {
        cv::cvtColor(mask, mask, cv::COLOR_GRAY2BGR);
    }
    
    cv::Mat img32;
    image.convertTo(img32, CV_32F);
    img32 = img32.mul(mask);
    
    cv::Mat out;
    img32.convertTo(out, CV_8U);
    return out;
}

std::vector<PFMSetting> PixelateFilter::defineSettings() const {
    return {
        { "size", "Pixel Size", SettingType::Integer, 10, SettingValue(), 2, 100, 2, 100, 1 }
    };
}
cv::Mat PixelateFilter::process(const cv::Mat& image) {
    int size = get("size").toInt();
    int h = image.rows;
    int w = image.cols;
    
    cv::Mat small, out;
    cv::resize(image, small, cv::Size(w / size, h / size), 0, 0, cv::INTER_LINEAR);
    cv::resize(small, out, cv::Size(w, h), 0, 0, cv::INTER_NEAREST);
    return out;
}

std::vector<PFMSetting> WaveFilter::defineSettings() const {
    return {
        { "amplitude", "Amplitude", SettingType::Number, 10.0, SettingValue(), 1.0, 100.0, 1.0, 100.0, 1.0 },
        { "frequency", "Frequency", SettingType::Number, 0.05, SettingValue(), 0.01, 0.5, 0.01, 0.5, 0.01 }
    };
}
cv::Mat WaveFilter::process(const cv::Mat& image) {
    int h = image.rows;
    int w = image.cols;
    double amp = get("amplitude").toDouble();
    double freq = get("frequency").toDouble();
    
    cv::Mat mapX(h, w, CV_32FC1);
    cv::Mat mapY(h, w, CV_32FC1);
    
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            mapY.at<float>(y, x) = static_cast<float>(y);
            mapX.at<float>(y, x) = static_cast<float>(x + amp * std::sin(y * freq));
        }
    }
    
    cv::Mat out;
    cv::remap(image, out, mapX, mapY, cv::INTER_LINEAR);
    return out;
}


// ===========================================================================
// MISSING SPEC FILTER IMPLEMENTATIONS
// ===========================================================================

// --- Borders ---
std::vector<PFMSetting> DirtyBorderFilter::defineSettings() const {
    return { {"width", "Border Width", SettingType::Integer, 15, SettingValue(), 1, 100, 1, 100, 1} };
}
cv::Mat DirtyBorderFilter::process(const cv::Mat& image) {
    int bw = get("width").toInt();
    cv::Mat out = image.clone();
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(0, 255);
    int h = out.rows, w = out.cols;
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
            if (x < bw || x >= w - bw || y < bw || y >= h - bw) {
                float fade = 1.0f - std::min({(float)x, (float)y, (float)(w-1-x), (float)(h-1-y)}) / bw;
                fade = std::clamp(fade, 0.0f, 1.0f);
                if (out.channels() == 1) {
                    uchar& px = out.at<uchar>(y, x);
                    px = cv::saturate_cast<uchar>(px * (1.0f - fade * 0.7f) + dist(rng) * fade * 0.3f);
                } else {
                    cv::Vec3b& px = out.at<cv::Vec3b>(y, x);
                    for (int c = 0; c < 3; ++c)
                        px[c] = cv::saturate_cast<uchar>(px[c] * (1.0f - fade * 0.7f) + dist(rng) * fade * 0.3f);
                }
            }
    return out;
}

std::vector<PFMSetting> CustomOverlayFilter::defineSettings() const {
    return { {"opacity", "Opacity", SettingType::Percentage, 50.0, SettingValue(), 0.0, 100.0, 0.0, 100.0, 1.0} };
}
cv::Mat CustomOverlayFilter::process(const cv::Mat& image) { return image.clone(); }

// --- Blur ---
std::vector<PFMSetting> GlowFilter::defineSettings() const {
    return { {"radius", "Radius", SettingType::Integer, 15, SettingValue(), 1, 101, 1, 101, 2},
             {"intensity", "Intensity", SettingType::Percentage, 50.0, SettingValue(), 0, 100, 0, 100, 1} };
}
cv::Mat GlowFilter::process(const cv::Mat& image) {
    int r = get("radius").toInt(); if (r % 2 == 0) r++;
    float i = get("intensity").toDouble() / 100.0f;
    cv::Mat blurred; cv::GaussianBlur(image, blurred, cv::Size(r, r), 0);
    cv::Mat out; cv::addWeighted(image, 1.0, blurred, i, 0, out);
    return out;
}

std::vector<PFMSetting> LensBlurFilter::defineSettings() const {
    return { {"radius", "Radius", SettingType::Integer, 11, SettingValue(), 3, 51, 3, 51, 2} };
}
cv::Mat LensBlurFilter::process(const cv::Mat& image) {
    int r = get("radius").toInt(); if (r % 2 == 0) r++;
    cv::Mat kernel = cv::Mat::zeros(r, r, CV_32F);
    cv::circle(kernel, cv::Point(r/2, r/2), r/2, cv::Scalar(1), -1);
    kernel /= cv::sum(kernel)[0];
    cv::Mat out; cv::filter2D(image, out, -1, kernel);
    return out;
}

std::vector<PFMSetting> MaximumFilter::defineSettings() const {
    return { {"ksize", "Kernel Size", SettingType::Integer, 3, SettingValue(), 1, 31, 1, 31, 2} };
}
cv::Mat MaximumFilter::process(const cv::Mat& image) {
    int k = get("ksize").toInt();
    cv::Mat el = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(k, k));
    cv::Mat out; cv::dilate(image, out, el);
    return out;
}

std::vector<PFMSetting> MinimumFilter::defineSettings() const {
    return { {"ksize", "Kernel Size", SettingType::Integer, 3, SettingValue(), 1, 31, 1, 31, 2} };
}
cv::Mat MinimumFilter::process(const cv::Mat& image) {
    int k = get("ksize").toInt();
    cv::Mat el = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(k, k));
    cv::Mat out; cv::erode(image, out, el);
    return out;
}

std::vector<PFMSetting> SmartBlurFilter::defineSettings() const {
    return { {"radius", "Radius", SettingType::Integer, 9, SettingValue(), 1, 99, 1, 99, 2},
             {"sigma_color", "Color Sigma", SettingType::Number, 75.0, SettingValue(), 1, 200, 1, 200, 1},
             {"sigma_space", "Space Sigma", SettingType::Number, 75.0, SettingValue(), 1, 200, 1, 200, 1} };
}
cv::Mat SmartBlurFilter::process(const cv::Mat& image) {
    int r = get("radius").toInt();
    float sc = get("sigma_color").toDouble(), ss = get("sigma_space").toDouble();
    cv::Mat out; cv::bilateralFilter(image, out, r, sc, ss);
    return out;
}

// --- Colors ---
std::vector<PFMSetting> AdjustHSBFilter::defineSettings() const {
    return { {"hue", "Hue", SettingType::Number, 0.0, SettingValue(), -180, 180, -180, 180, 1},
             {"saturation", "Saturation", SettingType::Number, 0.0, SettingValue(), -100, 100, -100, 100, 1},
             {"brightness", "Brightness", SettingType::Number, 0.0, SettingValue(), -100, 100, -100, 100, 1} };
}
cv::Mat AdjustHSBFilter::process(const cv::Mat& image) {
    if (image.channels() != 3) return image.clone();
    float dh = get("hue").toDouble(), ds = get("saturation").toDouble(), db = get("brightness").toDouble();
    cv::Mat hsv; cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);
    std::vector<cv::Mat> ch; cv::split(hsv, ch);
    ch[0].convertTo(ch[0], -1, 1.0, dh / 2.0);
    ch[1].convertTo(ch[1], -1, 1.0 + ds / 100.0);
    ch[2].convertTo(ch[2], -1, 1.0 + db / 100.0);
    cv::merge(ch, hsv);
    cv::Mat out; cv::cvtColor(hsv, out, cv::COLOR_HSV2BGR);
    return out;
}

std::vector<PFMSetting> AdjustRGBFilter::defineSettings() const {
    return { {"red", "Red", SettingType::Number, 0.0, SettingValue(), -100, 100, -100, 100, 1},
             {"green", "Green", SettingType::Number, 0.0, SettingValue(), -100, 100, -100, 100, 1},
             {"blue", "Blue", SettingType::Number, 0.0, SettingValue(), -100, 100, -100, 100, 1} };
}
cv::Mat AdjustRGBFilter::process(const cv::Mat& image) {
    if (image.channels() != 3) return image.clone();
    float dr = get("red").toDouble(), dg = get("green").toDouble(), db = get("blue").toDouble();
    std::vector<cv::Mat> ch; cv::split(image, ch);
    ch[0].convertTo(ch[0], -1, 1.0, db); ch[1].convertTo(ch[1], -1, 1.0, dg); ch[2].convertTo(ch[2], -1, 1.0, dr);
    cv::Mat out; cv::merge(ch, out);
    return out;
}

std::vector<PFMSetting> GainFilter::defineSettings() const {
    return { {"gain", "Gain", SettingType::Number, 1.0, SettingValue(), 0.0, 5.0, 0.0, 5.0, 0.1},
             {"bias", "Bias", SettingType::Number, 0.5, SettingValue(), 0.0, 1.0, 0.0, 1.0, 0.01} };
}
cv::Mat GainFilter::process(const cv::Mat& image) {
    float gain = get("gain").toDouble(), bias = get("bias").toDouble();
    cv::Mat out; image.convertTo(out, -1, gain, (bias - 0.5) * 255.0);
    return out;
}

std::vector<PFMSetting> GrayOutFilter::defineSettings() const {
    return { {"amount", "Amount", SettingType::Percentage, 50.0, SettingValue(), 0, 100, 0, 100, 1} };
}
cv::Mat GrayOutFilter::process(const cv::Mat& image) {
    float a = get("amount").toDouble() / 100.0f;
    cv::Mat gray;
    if (image.channels() == 3) { cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY); cv::cvtColor(gray, gray, cv::COLOR_GRAY2BGR); }
    else gray = image.clone();
    cv::Mat out; cv::addWeighted(image, 1.0 - a, gray, a, 0, out);
    return out;
}

std::vector<PFMSetting> LevelsFilter::defineSettings() const {
    return { {"input_low", "Input Low", SettingType::Integer, 0, SettingValue(), 0, 255, 0, 255, 1},
             {"input_high", "Input High", SettingType::Integer, 255, SettingValue(), 0, 255, 0, 255, 1},
             {"output_low", "Output Low", SettingType::Integer, 0, SettingValue(), 0, 255, 0, 255, 1},
             {"output_high", "Output High", SettingType::Integer, 255, SettingValue(), 0, 255, 0, 255, 1} };
}
cv::Mat LevelsFilter::process(const cv::Mat& image) {
    int il = get("input_low").toInt(), ih = get("input_high").toInt();
    int ol = get("output_low").toInt(), oh = get("output_high").toInt();
    if (ih <= il) ih = il + 1;
    cv::Mat img32; image.convertTo(img32, CV_32F);
    img32 = (img32 - il) / (ih - il) * (oh - ol) + ol;
    cv::Mat out; img32.convertTo(out, CV_8U);
    return out;
}

std::vector<PFMSetting> MixChannelsFilter::defineSettings() const {
    return { {"mode", "Mode", SettingType::Enum, std::string("RGB"), SettingValue(), 0, 0, 0, 0, 1, {"RGB", "RBG", "GRB", "GBR", "BRG", "BGR"}} };
}
cv::Mat MixChannelsFilter::process(const cv::Mat& image) {
    if (image.channels() != 3) return image.clone();
    std::string mode = get("mode").toString();
    std::vector<cv::Mat> ch; cv::split(image, ch); // BGR
    std::vector<cv::Mat> out_ch = {ch[0], ch[1], ch[2]};
    if (mode == "RBG") out_ch = {ch[1], ch[0], ch[2]};
    else if (mode == "GRB") out_ch = {ch[0], ch[2], ch[1]};
    else if (mode == "GBR") out_ch = {ch[2], ch[0], ch[1]};
    else if (mode == "BRG") out_ch = {ch[1], ch[2], ch[0]};
    cv::Mat out; cv::merge(out_ch, out);
    return out;
}

std::vector<PFMSetting> RescaleFilter::defineSettings() const {
    return { {"scale", "Scale", SettingType::Number, 1.0, SettingValue(), 0.1, 4.0, 0.1, 4.0, 0.1} };
}
cv::Mat RescaleFilter::process(const cv::Mat& image) {
    float s = get("scale").toDouble();
    cv::Mat out; cv::resize(image, out, cv::Size(), s, s, cv::INTER_LINEAR);
    return out;
}

std::vector<PFMSetting> SolarizeFilter::defineSettings() const {
    return { {"threshold", "Threshold", SettingType::Integer, 128, SettingValue(), 0, 255, 0, 255, 1} };
}
cv::Mat SolarizeFilter::process(const cv::Mat& image) {
    int t = get("threshold").toInt();
    cv::Mat out = image.clone();
    for (int y = 0; y < out.rows; ++y)
        for (int x = 0; x < out.cols * out.channels(); ++x) {
            uchar& v = out.data[y * out.step + x];
            if (v > t) v = 255 - v;
        }
    return out;
}

std::vector<PFMSetting> TransparencyFilter::defineSettings() const {
    return { {"opacity", "Opacity", SettingType::Percentage, 50.0, SettingValue(), 0, 100, 0, 100, 1} };
}
cv::Mat TransparencyFilter::process(const cv::Mat& image) {
    float a = get("opacity").toDouble() / 100.0f;
    cv::Mat white(image.size(), image.type(), cv::Scalar::all(255));
    cv::Mat out; cv::addWeighted(image, a, white, 1.0 - a, 0, out);
    return out;
}

// --- Distort ---
std::vector<PFMSetting> DiffuseFilter::defineSettings() const {
    return { {"scale", "Scale", SettingType::Number, 4.0, SettingValue(), 1, 20, 1, 20, 1} };
}
cv::Mat DiffuseFilter::process(const cv::Mat& image) {
    float s = get("scale").toDouble();
    int h = image.rows, w = image.cols;
    cv::Mat mx(h, w, CV_32FC1), my(h, w, CV_32FC1);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-s, s);
    for (int y = 0; y < h; ++y) for (int x = 0; x < w; ++x) {
        mx.at<float>(y, x) = x + dist(rng); my.at<float>(y, x) = y + dist(rng);
    }
    cv::Mat out; cv::remap(image, out, mx, my, cv::INTER_LINEAR, cv::BORDER_REFLECT);
    return out;
}

std::vector<PFMSetting> DisplaceFilter::defineSettings() const {
    return { {"amount", "Amount", SettingType::Number, 10.0, SettingValue(), 1, 100, 1, 100, 1} };
}
cv::Mat DisplaceFilter::process(const cv::Mat& image) {
    float a = get("amount").toDouble();
    cv::Mat gray; if (image.channels() == 3) cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY); else gray = image;
    int h = image.rows, w = image.cols;
    cv::Mat mx(h, w, CV_32FC1), my(h, w, CV_32FC1);
    cv::Mat gx, gy; cv::Sobel(gray, gx, CV_32F, 1, 0, 3); cv::Sobel(gray, gy, CV_32F, 0, 1, 3);
    for (int y = 0; y < h; ++y) for (int x = 0; x < w; ++x) {
        mx.at<float>(y, x) = x + gx.at<float>(y, x) / 255.0f * a;
        my.at<float>(y, x) = y + gy.at<float>(y, x) / 255.0f * a;
    }
    cv::Mat out; cv::remap(image, out, mx, my, cv::INTER_LINEAR, cv::BORDER_REFLECT);
    return out;
}

std::vector<PFMSetting> KaleidoscopeFilter::defineSettings() const {
    return { {"segments", "Segments", SettingType::Integer, 6, SettingValue(), 2, 24, 2, 24, 1} };
}
cv::Mat KaleidoscopeFilter::process(const cv::Mat& image) {
    int segs = get("segments").toInt();
    int h = image.rows, w = image.cols;
    float cx = w / 2.0f, cy = h / 2.0f, seg_angle = 2.0f * 3.14159265f / segs;
    cv::Mat mx(h, w, CV_32FC1), my(h, w, CV_32FC1);
    for (int y = 0; y < h; ++y) for (int x = 0; x < w; ++x) {
        float dx = x - cx, dy = y - cy;
        float angle = std::atan2(dy, dx); if (angle < 0) angle += 2.0f * 3.14159265f;
        float r = std::hypot(dx, dy);
        float sa = std::fmod(angle, seg_angle);
        if (sa > seg_angle / 2.0f) sa = seg_angle - sa;
        mx.at<float>(y, x) = cx + r * std::cos(sa);
        my.at<float>(y, x) = cy + r * std::sin(sa);
    }
    cv::Mat out; cv::remap(image, out, mx, my, cv::INTER_LINEAR, cv::BORDER_REFLECT);
    return out;
}

std::vector<PFMSetting> MarbleFilter::defineSettings() const {
    return { {"scale", "Scale", SettingType::Number, 10.0, SettingValue(), 1, 100, 1, 100, 1},
             {"turbulence", "Turbulence", SettingType::Number, 4.0, SettingValue(), 0.1, 20, 0.1, 20, 0.1} };
}
cv::Mat MarbleFilter::process(const cv::Mat& image) {
    float scale = get("scale").toDouble(), turb = get("turbulence").toDouble();
    int h = image.rows, w = image.cols;
    cv::Mat mx(h, w, CV_32FC1), my(h, w, CV_32FC1);
    for (int y = 0; y < h; ++y) for (int x = 0; x < w; ++x) {
        float v = std::sin((x + y) / scale + turb * std::sin(x * 0.03f) + turb * std::cos(y * 0.03f));
        mx.at<float>(y, x) = x + v * scale; my.at<float>(y, x) = y + v * scale;
    }
    cv::Mat out; cv::remap(image, out, mx, my, cv::INTER_LINEAR, cv::BORDER_REFLECT);
    return out;
}

std::vector<PFMSetting> RippleFilter::defineSettings() const {
    return { {"amplitude", "Amplitude", SettingType::Number, 5.0, SettingValue(), 1, 50, 1, 50, 1},
             {"wavelength", "Wavelength", SettingType::Number, 20.0, SettingValue(), 1, 200, 1, 200, 1} };
}
cv::Mat RippleFilter::process(const cv::Mat& image) {
    float amp = get("amplitude").toDouble(), wl = get("wavelength").toDouble();
    int h = image.rows, w = image.cols;
    cv::Mat mx(h, w, CV_32FC1), my(h, w, CV_32FC1);
    for (int y = 0; y < h; ++y) for (int x = 0; x < w; ++x) {
        mx.at<float>(y, x) = x + amp * std::sin(2.0f * 3.14159265f * y / wl);
        my.at<float>(y, x) = y + amp * std::cos(2.0f * 3.14159265f * x / wl);
    }
    cv::Mat out; cv::remap(image, out, mx, my, cv::INTER_LINEAR, cv::BORDER_REFLECT);
    return out;
}

std::vector<PFMSetting> ShearFilter::defineSettings() const {
    return { {"angle", "Angle", SettingType::Number, 15.0, SettingValue(), -45, 45, -45, 45, 1} };
}
cv::Mat ShearFilter::process(const cv::Mat& image) {
    float a = std::tan(get("angle").toDouble() * 3.14159265f / 180.0f);
    cv::Mat M = (cv::Mat_<float>(2, 3) << 1, a, 0, 0, 1, 0);
    cv::Mat out; cv::warpAffine(image, out, M, image.size(), cv::INTER_LINEAR, cv::BORDER_REFLECT);
    return out;
}

std::vector<PFMSetting> SwimFilter::defineSettings() const {
    return { {"scale", "Scale", SettingType::Number, 16.0, SettingValue(), 1, 100, 1, 100, 1},
             {"amount", "Amount", SettingType::Number, 8.0, SettingValue(), 1, 50, 1, 50, 1} };
}
cv::Mat SwimFilter::process(const cv::Mat& image) {
    float sc = get("scale").toDouble(), amt = get("amount").toDouble();
    int h = image.rows, w = image.cols;
    cv::Mat mx(h, w, CV_32FC1), my(h, w, CV_32FC1);
    for (int y = 0; y < h; ++y) for (int x = 0; x < w; ++x) {
        mx.at<float>(y, x) = x + amt * std::sin(y / sc);
        my.at<float>(y, x) = y + amt * std::cos(x / sc);
    }
    cv::Mat out; cv::remap(image, out, mx, my, cv::INTER_LINEAR, cv::BORDER_REFLECT);
    return out;
}

// --- Effects ---
std::vector<PFMSetting> ChromeFilter::defineSettings() const { return {}; }
cv::Mat ChromeFilter::process(const cv::Mat& image) {
    cv::Mat gray; if (image.channels() == 3) cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY); else gray = image.clone();
    cv::Mat gx, gy; cv::Sobel(gray, gx, CV_32F, 1, 0); cv::Sobel(gray, gy, CV_32F, 0, 1);
    cv::Mat mag; cv::magnitude(gx, gy, mag);
    cv::normalize(mag, mag, 0, 255, cv::NORM_MINMAX);
    cv::Mat out; mag.convertTo(out, CV_8U);
    if (image.channels() == 3) cv::cvtColor(out, out, cv::COLOR_GRAY2BGR);
    return out;
}

std::vector<PFMSetting> FeedbackFilter::defineSettings() const {
    return { {"iterations", "Iterations", SettingType::Integer, 3, SettingValue(), 1, 10, 1, 10, 1},
             {"scale", "Scale", SettingType::Number, 0.95, SettingValue(), 0.5, 1.0, 0.5, 1.0, 0.01} };
}
cv::Mat FeedbackFilter::process(const cv::Mat& image) {
    int iters = get("iterations").toInt(); float s = get("scale").toDouble();
    cv::Mat out = image.clone();
    for (int i = 0; i < iters; ++i) {
        cv::Mat resized; cv::resize(out, resized, cv::Size(), s, s);
        cv::Mat centered(image.size(), image.type(), cv::Scalar::all(128));
        int ox = (image.cols - resized.cols) / 2, oy = (image.rows - resized.rows) / 2;
        if (ox >= 0 && oy >= 0) resized.copyTo(centered(cv::Rect(ox, oy, resized.cols, resized.rows)));
        cv::addWeighted(out, 0.5, centered, 0.5, 0, out);
    }
    return out;
}

std::vector<PFMSetting> GlintFilter::defineSettings() const {
    return { {"threshold", "Threshold", SettingType::Integer, 230, SettingValue(), 0, 255, 0, 255, 1},
             {"length", "Length", SettingType::Integer, 20, SettingValue(), 1, 100, 1, 100, 1} };
}
cv::Mat GlintFilter::process(const cv::Mat& image) {
    int t = get("threshold").toInt(), len = get("length").toInt();
    cv::Mat gray; if (image.channels() == 3) cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY); else gray = image;
    cv::Mat bright; cv::threshold(gray, bright, t, 255, cv::THRESH_BINARY);
    cv::Mat kh = cv::Mat::zeros(1, len * 2 + 1, CV_32F); kh.at<float>(0, len) = 1;
    for (int i = 0; i < len * 2 + 1; ++i) kh.at<float>(0, i) = 1.0f / (len * 2 + 1);
    cv::Mat kv; cv::transpose(kh, kv);
    cv::Mat gh, gv; cv::filter2D(bright, gh, -1, kh); cv::filter2D(bright, gv, -1, kv);
    cv::Mat glints = cv::max(gh, gv);
    if (image.channels() == 3) cv::cvtColor(glints, glints, cv::COLOR_GRAY2BGR);
    cv::Mat out; cv::add(image, glints, out);
    return out;
}

std::vector<PFMSetting> MirrorFilter::defineSettings() const {
    return { {"mode", "Mode", SettingType::Enum, std::string("Horizontal"), SettingValue(), 0, 0, 0, 0, 1, {"Horizontal", "Vertical", "Both"}} };
}
cv::Mat MirrorFilter::process(const cv::Mat& image) {
    std::string m = get("mode").toString();
    cv::Mat out;
    if (m == "Vertical") cv::flip(image, out, 0);
    else if (m == "Both") cv::flip(image, out, -1);
    else cv::flip(image, out, 1);
    return out;
}

// --- Keying ---
std::vector<PFMSetting> ChromaKeyFilter::defineSettings() const {
    return { {"hue_target", "Target Hue", SettingType::Integer, 60, SettingValue(), 0, 180, 0, 180, 1},
             {"tolerance", "Tolerance", SettingType::Integer, 30, SettingValue(), 1, 90, 1, 90, 1} };
}
cv::Mat ChromaKeyFilter::process(const cv::Mat& image) {
    if (image.channels() != 3) return image.clone();
    int hue = get("hue_target").toInt(), tol = get("tolerance").toInt();
    cv::Mat hsv; cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);
    cv::Mat mask; cv::inRange(hsv, cv::Scalar(hue - tol, 40, 40), cv::Scalar(hue + tol, 255, 255), mask);
    cv::Mat out = image.clone(); out.setTo(cv::Scalar(255, 255, 255), mask);
    return out;
}

// --- Pixellate ---
std::vector<PFMSetting> ColorHalftoneFilter::defineSettings() const {
    return { {"dot_size", "Dot Size", SettingType::Integer, 8, SettingValue(), 2, 40, 2, 40, 1} };
}
cv::Mat ColorHalftoneFilter::process(const cv::Mat& image) {
    int ds = get("dot_size").toInt();
    cv::Mat gray; if (image.channels() == 3) cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY); else gray = image;
    int h = gray.rows, w = gray.cols;
    cv::Mat out(h, w, CV_8U, cv::Scalar(255));
    for (int y = 0; y < h; y += ds) for (int x = 0; x < w; x += ds) {
        cv::Rect r(x, y, std::min(ds, w - x), std::min(ds, h - y));
        float m = cv::mean(gray(r))[0];
        float radius = (255.0f - m) / 255.0f * ds / 2.0f;
        cv::circle(out, cv::Point(x + ds/2, y + ds/2), (int)radius, cv::Scalar(0), -1);
    }
    return out;
}

std::vector<PFMSetting> CrystallizeFilter::defineSettings() const {
    return { {"cell_size", "Cell Size", SettingType::Integer, 15, SettingValue(), 2, 100, 2, 100, 1} };
}
cv::Mat CrystallizeFilter::process(const cv::Mat& image) {
    int cs = get("cell_size").toInt();
    int h = image.rows, w = image.cols;
    cv::Mat out = image.clone();
    for (int y = 0; y < h; y += cs) for (int x = 0; x < w; x += cs) {
        cv::Rect r(x, y, std::min(cs, w - x), std::min(cs, h - y));
        cv::Scalar m = cv::mean(image(r));
        out(r).setTo(m);
    }
    return out;
}

std::vector<PFMSetting> PointillizeFilter::defineSettings() const {
    return { {"cell_size", "Cell Size", SettingType::Integer, 10, SettingValue(), 2, 50, 2, 50, 1} };
}
cv::Mat PointillizeFilter::process(const cv::Mat& image) {
    int cs = get("cell_size").toInt();
    int h = image.rows, w = image.cols;
    cv::Mat out(h, w, image.type(), cv::Scalar::all(255));
    for (int y = 0; y < h; y += cs) for (int x = 0; x < w; x += cs) {
        cv::Rect r(x, y, std::min(cs, w - x), std::min(cs, h - y));
        cv::Scalar m = cv::mean(image(r));
        cv::circle(out, cv::Point(x + cs/2, y + cs/2), cs/2, m, -1);
    }
    return out;
}

// --- Render ---
std::vector<PFMSetting> ScratchesFilter::defineSettings() const {
    return { {"count", "Count", SettingType::Integer, 30, SettingValue(), 1, 200, 1, 200, 1},
             {"length", "Length", SettingType::Integer, 100, SettingValue(), 10, 500, 10, 500, 10} };
}
cv::Mat ScratchesFilter::process(const cv::Mat& image) {
    int cnt = get("count").toInt(), len = get("length").toInt();
    cv::Mat out = image.clone();
    std::mt19937 rng(42);
    for (int i = 0; i < cnt; ++i) {
        int x1 = rng() % image.cols, y1 = rng() % image.rows;
        float a = (rng() % 360) * 3.14159265f / 180.0f;
        int x2 = x1 + (int)(len * std::cos(a)), y2 = y1 + (int)(len * std::sin(a));
        cv::Scalar color = (image.channels() == 3) ? cv::Scalar(200, 200, 200) : cv::Scalar(200);
        cv::line(out, cv::Point(x1, y1), cv::Point(x2, y2), color, 1, cv::LINE_AA);
    }
    return out;
}

// --- Stylize ---
std::vector<PFMSetting> ContoursFilter::defineSettings() const {
    return { {"levels", "Levels", SettingType::Integer, 8, SettingValue(), 2, 30, 2, 30, 1} };
}
cv::Mat ContoursFilter::process(const cv::Mat& image) {
    int levels = get("levels").toInt();
    cv::Mat gray; if (image.channels() == 3) cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY); else gray = image.clone();
    cv::Mat out = cv::Mat::zeros(gray.size(), CV_8U);
    for (int l = 1; l < levels; ++l) {
        int t = 255 * l / levels;
        cv::Mat bin; cv::threshold(gray, bin, t, 255, cv::THRESH_BINARY);
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(bin, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
        cv::drawContours(out, contours, -1, cv::Scalar(255), 1);
    }
    cv::bitwise_not(out, out);
    return out;
}

std::vector<PFMSetting> DissolveFilter::defineSettings() const {
    return { {"density", "Density", SettingType::Percentage, 20.0, SettingValue(), 0, 100, 0, 100, 1} };
}
cv::Mat DissolveFilter::process(const cv::Mat& image) {
    float d = get("density").toDouble() / 100.0f;
    cv::Mat out = image.clone();
    cv::Mat noise(image.size(), CV_32F); cv::randu(noise, 0.0, 1.0);
    cv::Mat mask = noise < d;
    out.setTo(cv::Scalar::all(255), mask);
    return out;
}

std::vector<PFMSetting> DropShadowFilter::defineSettings() const {
    return { {"offset_x", "Offset X", SettingType::Integer, 5, SettingValue(), -50, 50, -50, 50, 1},
             {"offset_y", "Offset Y", SettingType::Integer, 5, SettingValue(), -50, 50, -50, 50, 1},
             {"blur", "Blur", SettingType::Integer, 5, SettingValue(), 0, 50, 0, 50, 1} };
}
cv::Mat DropShadowFilter::process(const cv::Mat& image) {
    int ox = get("offset_x").toInt(), oy = get("offset_y").toInt(), bl = get("blur").toInt();
    if (bl % 2 == 0) bl++;
    cv::Mat dark; image.convertTo(dark, -1, 0.3);
    cv::Mat shadow(image.size(), image.type(), cv::Scalar::all(255));
    int sx = std::max(0, ox), sy = std::max(0, oy);
    int dw = std::min(image.cols - sx, image.cols - std::abs(ox));
    int dh = std::min(image.rows - sy, image.rows - std::abs(oy));
    if (dw > 0 && dh > 0) dark(cv::Rect(std::max(0, -ox), std::max(0, -oy), dw, dh)).copyTo(shadow(cv::Rect(sx, sy, dw, dh)));
    if (bl > 1) cv::GaussianBlur(shadow, shadow, cv::Size(bl, bl), 0);
    cv::Mat out; cv::min(shadow, image, out);
    return out;
}

std::vector<PFMSetting> FlareFilter::defineSettings() const {
    return { {"center_x", "Center X", SettingType::Percentage, 50.0, SettingValue(), 0, 100, 0, 100, 1},
             {"center_y", "Center Y", SettingType::Percentage, 50.0, SettingValue(), 0, 100, 0, 100, 1},
             {"intensity", "Intensity", SettingType::Percentage, 80.0, SettingValue(), 0, 100, 0, 100, 1} };
}
cv::Mat FlareFilter::process(const cv::Mat& image) {
    float cx = get("center_x").toDouble() / 100.0f * image.cols;
    float cy = get("center_y").toDouble() / 100.0f * image.rows;
    float intensity = get("intensity").toDouble() / 100.0f;
    cv::Mat flare(image.size(), CV_32F);
    float max_d = std::hypot((float)image.cols, (float)image.rows) / 2.0f;
    for (int y = 0; y < image.rows; ++y) for (int x = 0; x < image.cols; ++x) {
        float d = std::hypot(x - cx, y - cy);
        flare.at<float>(y, x) = std::max(0.0f, 1.0f - d / max_d) * intensity * 255.0f;
    }
    cv::Mat fm; flare.convertTo(fm, CV_8U);
    if (image.channels() == 3) cv::cvtColor(fm, fm, cv::COLOR_GRAY2BGR);
    cv::Mat out; cv::add(image, fm, out);
    return out;
}

std::vector<PFMSetting> OilFilter::defineSettings() const {
    return { 
        {"radius", "Radius", SettingType::Integer, 3, SettingValue(), 1, 10, 1, 10, 1},
        {"sigma_s", "Sigma S", SettingType::Number, 60.0, SettingValue(), 1.0, 200.0, 1.0, 200.0, 1.0},
        {"sigma_r", "Sigma R", SettingType::Number, 0.4, SettingValue(), 0.01, 1.0, 0.01, 1.0, 0.01}
    };
}
cv::Mat OilFilter::process(const cv::Mat& image) {
    // Delegate to OilPaintingFilter logic (simplified version)
    float sigmaS = (float)get("sigma_s").toDouble();
    float sigmaR = (float)get("sigma_r").toDouble();
    cv::Mat bgr;
    if (image.channels() == 1) cv::cvtColor(image, bgr, cv::COLOR_GRAY2BGR); else bgr = image;
    cv::Mat out;
    cv::edgePreservingFilter(bgr, out, 1, sigmaS, sigmaR);
    if (image.channels() == 1) cv::cvtColor(out, out, cv::COLOR_BGR2GRAY);
    return out;
}

std::vector<PFMSetting> RaysFilter::defineSettings() const {
    return { {"count", "Ray Count", SettingType::Integer, 12, SettingValue(), 2, 60, 2, 60, 1},
             {"length", "Length", SettingType::Integer, 50, SettingValue(), 10, 200, 10, 200, 5} };
}
cv::Mat RaysFilter::process(const cv::Mat& image) {
    int cnt = get("count").toInt(), len = get("length").toInt();
    cv::Mat out = image.clone();
    float cx = image.cols / 2.0f, cy = image.rows / 2.0f;
    for (int i = 0; i < cnt; ++i) {
        float a = 2.0f * 3.14159265f * i / cnt;
        cv::Scalar c = (image.channels() == 3) ? cv::Scalar(255, 255, 240) : cv::Scalar(255);
        cv::line(out, cv::Point((int)cx, (int)cy),
                 cv::Point((int)(cx + len * std::cos(a)), (int)(cy + len * std::sin(a))), c, 1, cv::LINE_AA);
    }
    return out;
}

std::vector<PFMSetting> ShapeBurstFilter::defineSettings() const {
    return {
        { "threshold", "Threshold", SettingType::Integer, 128, SettingValue(), 0, 255, 0, 255, 1 }
    };
}
cv::Mat ShapeBurstFilter::process(const cv::Mat& image) {
    int t = get("threshold").toInt();
    cv::Mat gray; if (image.channels() == 3) cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY); else gray = image;
    cv::Mat bin; cv::threshold(gray, bin, t, 255, cv::THRESH_BINARY);
    cv::Mat dist; cv::distanceTransform(bin, dist, cv::DIST_L2, 3);
    cv::normalize(dist, dist, 0, 255, cv::NORM_MINMAX);
    cv::Mat out; dist.convertTo(out, CV_8U);
    return out;
}

std::vector<PFMSetting> SparkleFilter::defineSettings() const {
    return { {"density", "Density", SettingType::Integer, 50, SettingValue(), 1, 500, 1, 500, 5},
             {"size", "Size", SettingType::Integer, 3, SettingValue(), 1, 10, 1, 10, 1} };
}
cv::Mat SparkleFilter::process(const cv::Mat& image) {
    int d = get("density").toInt(), s = get("size").toInt();
    cv::Mat out = image.clone();
    std::mt19937 rng(42);
    cv::Scalar c = (image.channels() == 3) ? cv::Scalar(255, 255, 255) : cv::Scalar(255);
    for (int i = 0; i < d; ++i) {
        int x = rng() % image.cols, y = rng() % image.rows;
        cv::line(out, cv::Point(x - s, y), cv::Point(x + s, y), c, 1);
        cv::line(out, cv::Point(x, y - s), cv::Point(x, y + s), c, 1);
    }
    return out;
}

std::vector<PFMSetting> StampFilter::defineSettings() const {
    return { {"threshold", "Threshold", SettingType::Integer, 128, SettingValue(), 0, 255, 0, 255, 1},
             {"smoothness", "Smoothness", SettingType::Integer, 3, SettingValue(), 0, 20, 0, 20, 1} };
}
cv::Mat StampFilter::process(const cv::Mat& image) {
    int t = get("threshold").toInt(), sm = get("smoothness").toInt();
    cv::Mat gray; if (image.channels() == 3) cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY); else gray = image;
    if (sm > 0) { int k = sm * 2 + 1; cv::GaussianBlur(gray, gray, cv::Size(k, k), 0); }
    cv::Mat out; cv::threshold(gray, out, t, 255, cv::THRESH_BINARY);
    return out;
}


std::unique_ptr<ImageFilter> create_filter(const std::string& name) {
    if (name == "ThresholdFilter") return std::make_unique<ThresholdFilter>();
    if (name == "BrightnessFilter") return std::make_unique<BrightnessFilter>();
    if (name == "ContrastFilter") return std::make_unique<ContrastFilter>();
    if (name == "InvertFilter") return std::make_unique<InvertFilter>();
    if (name == "UnsharpMaskFilter") return std::make_unique<UnsharpMaskFilter>();
    if (name == "GrayscaleFilter") return std::make_unique<GrayscaleFilter>();
    if (name == "DesaturateFilter") return std::make_unique<DesaturateFilter>();
    if (name == "SaturationFilter") return std::make_unique<SaturationFilter>();
    if (name == "HueFilter") return std::make_unique<HueFilter>();
    if (name == "GammaFilter") return std::make_unique<GammaFilter>();
    if (name == "ExposureFilter") return std::make_unique<ExposureFilter>();
    if (name == "SepiaFilter") return std::make_unique<SepiaFilter>();
    if (name == "CLAHEFilter") return std::make_unique<CLAHEFilter>();
    if (name == "TemperatureFilter") return std::make_unique<TemperatureFilter>();
    if (name == "CannyFilter") return std::make_unique<CannyFilter>();
    if (name == "SobelFilter") return std::make_unique<SobelFilter>();
    if (name == "LaplacianFilter") return std::make_unique<LaplacianFilter>();
    if (name == "PrewittFilter") return std::make_unique<PrewittFilter>();
    if (name == "ScharrFilter") return std::make_unique<ScharrFilter>();
    if (name == "DoGFilter") return std::make_unique<DoGFilter>();
    if (name == "RidgeDetectionFilter") return std::make_unique<RidgeDetectionFilter>();
    if (name == "HighPassFilter") return std::make_unique<HighPassFilter>();
    if (name == "GaussianBlurFilter") return std::make_unique<GaussianBlurFilter>();
    if (name == "MedianBlurFilter") return std::make_unique<MedianBlurFilter>();
    if (name == "BoxBlurFilter") return std::make_unique<BoxBlurFilter>();
    if (name == "MotionBlurFilter") return std::make_unique<MotionBlurFilter>();
    if (name == "BilateralFilter") return std::make_unique<BilateralFilter>();
    if (name == "LowPassFilter") return std::make_unique<LowPassFilter>();
    if (name == "SharpenMoreFilter") return std::make_unique<SharpenMoreFilter>();
    if (name == "DilationFilter") return std::make_unique<DilationFilter>();
    if (name == "ErosionFilter") return std::make_unique<ErosionFilter>();
    if (name == "OpeningFilter") return std::make_unique<OpeningFilter>();
    if (name == "ClosingFilter") return std::make_unique<ClosingFilter>();
    if (name == "MorphGradientFilter") return std::make_unique<MorphGradientFilter>();
    if (name == "TopHatFilter") return std::make_unique<TopHatFilter>();
    if (name == "BlackHatFilter") return std::make_unique<BlackHatFilter>();
    if (name == "DilateCrossFilter") return std::make_unique<DilateCrossFilter>();
    if (name == "ErodeCrossFilter") return std::make_unique<ErodeCrossFilter>();
    if (name == "DilateEllipseFilter") return std::make_unique<DilateEllipseFilter>();
    if (name == "ErodeEllipseFilter") return std::make_unique<ErodeEllipseFilter>();
    if (name == "GaussianNoiseFilter") return std::make_unique<GaussianNoiseFilter>();
    if (name == "SaltAndPepperFilter") return std::make_unique<SaltAndPepperFilter>();
    if (name == "DenoiseFilter") return std::make_unique<DenoiseFilter>();
    if (name == "GaussianNoise2Filter") return std::make_unique<GaussianNoise2Filter>();
    if (name == "SpeckleNoiseFilter") return std::make_unique<SpeckleNoiseFilter>();
    if (name == "OtsuThresholdFilter") return std::make_unique<OtsuThresholdFilter>();
    if (name == "AdaptiveThresholdFilter") return std::make_unique<AdaptiveThresholdFilter>();
    if (name == "TruncateThresholdFilter") return std::make_unique<TruncateThresholdFilter>();
    if (name == "ToZeroThresholdFilter") return std::make_unique<ToZeroThresholdFilter>();
    if (name == "EqualizeHistFilter") return std::make_unique<EqualizeHistFilter>();
    if (name == "AutoContrastFilter") return std::make_unique<AutoContrastFilter>();
    if (name == "AutoColorFilter") return std::make_unique<AutoColorFilter>();
    if (name == "ColorizeFilter") return std::make_unique<ColorizeFilter>();
    if (name == "InvertHueFilter") return std::make_unique<InvertHueFilter>();
    if (name == "PosterizeFilter") return std::make_unique<PosterizeFilter>();
    if (name == "EdgePreserveFilter") return std::make_unique<EdgePreserveFilter>();
    if (name == "StylizationFilter") return std::make_unique<StylizationFilter>();
    if (name == "OilPaintingFilter") return std::make_unique<OilPaintingFilter>();
    if (name == "DetailEnhanceFilter") return std::make_unique<DetailEnhanceFilter>();
    if (name == "PencilSketchFilter") return std::make_unique<PencilSketchFilter>();
    if (name == "EmbossFilter") return std::make_unique<EmbossFilter>();
    if (name == "QuantizeFilter") return std::make_unique<QuantizeFilter>();
    if (name == "VignetteFilter") return std::make_unique<VignetteFilter>();
    if (name == "PixelateFilter") return std::make_unique<PixelateFilter>();
    if (name == "WaveFilter") return std::make_unique<WaveFilter>();
    // --- New spec filters ---
    if (name == "DirtyBorderFilter") return std::make_unique<DirtyBorderFilter>();
    if (name == "CustomOverlayFilter") return std::make_unique<CustomOverlayFilter>();
    if (name == "GlowFilter") return std::make_unique<GlowFilter>();
    if (name == "LensBlurFilter") return std::make_unique<LensBlurFilter>();
    if (name == "MaximumFilter") return std::make_unique<MaximumFilter>();
    if (name == "MinimumFilter") return std::make_unique<MinimumFilter>();
    if (name == "SmartBlurFilter") return std::make_unique<SmartBlurFilter>();
    if (name == "AdjustHSBFilter") return std::make_unique<AdjustHSBFilter>();
    if (name == "AdjustRGBFilter") return std::make_unique<AdjustRGBFilter>();
    if (name == "GainFilter") return std::make_unique<GainFilter>();
    if (name == "GrayOutFilter") return std::make_unique<GrayOutFilter>();
    if (name == "LevelsFilter") return std::make_unique<LevelsFilter>();
    if (name == "MixChannelsFilter") return std::make_unique<MixChannelsFilter>();
    if (name == "RescaleFilter") return std::make_unique<RescaleFilter>();
    if (name == "SolarizeFilter") return std::make_unique<SolarizeFilter>();
    if (name == "TransparencyFilter") return std::make_unique<TransparencyFilter>();
    if (name == "DiffuseFilter") return std::make_unique<DiffuseFilter>();
    if (name == "DisplaceFilter") return std::make_unique<DisplaceFilter>();
    if (name == "KaleidoscopeFilter") return std::make_unique<KaleidoscopeFilter>();
    if (name == "MarbleFilter") return std::make_unique<MarbleFilter>();
    if (name == "RippleFilter") return std::make_unique<RippleFilter>();
    if (name == "ShearFilter") return std::make_unique<ShearFilter>();
    if (name == "SwimFilter") return std::make_unique<SwimFilter>();
    if (name == "ChromeFilter") return std::make_unique<ChromeFilter>();
    if (name == "FeedbackFilter") return std::make_unique<FeedbackFilter>();
    if (name == "GlintFilter") return std::make_unique<GlintFilter>();
    if (name == "MirrorFilter") return std::make_unique<MirrorFilter>();
    if (name == "ChromaKeyFilter") return std::make_unique<ChromaKeyFilter>();
    if (name == "ColorHalftoneFilter") return std::make_unique<ColorHalftoneFilter>();
    if (name == "CrystallizeFilter") return std::make_unique<CrystallizeFilter>();
    if (name == "PointillizeFilter") return std::make_unique<PointillizeFilter>();
    if (name == "ScratchesFilter") return std::make_unique<ScratchesFilter>();
    if (name == "ContoursFilter") return std::make_unique<ContoursFilter>();
    if (name == "DissolveFilter") return std::make_unique<DissolveFilter>();
    if (name == "DropShadowFilter") return std::make_unique<DropShadowFilter>();
    if (name == "FlareFilter") return std::make_unique<FlareFilter>();
    if (name == "OilFilter") return std::make_unique<OilFilter>();
    if (name == "RaysFilter") return std::make_unique<RaysFilter>();
    if (name == "ShapeBurstFilter") return std::make_unique<ShapeBurstFilter>();
    if (name == "SparkleFilter") return std::make_unique<SparkleFilter>();
    if (name == "StampFilter") return std::make_unique<StampFilter>();
    return nullptr;
}
