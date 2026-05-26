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
    
    cv::Mat out;
    cv::transform(img3, out, kernel);
    out.convertTo(out, CV_8U);
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

cv::Mat HighPassFilter::process(const cv::Mat& image) {
    cv::Mat blur;
    cv::GaussianBlur(image, blur, cv::Size(11, 11), 0);
    
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

cv::Mat BilateralFilter::process(const cv::Mat& image) {
    cv::Mat out;
    cv::bilateralFilter(image, out, 9, 75, 75);
    return out;
}

cv::Mat LowPassFilter::process(const cv::Mat& image) {
    cv::Mat out;
    cv::GaussianBlur(image, out, cv::Size(21, 21), 0);
    return out;
}

cv::Mat SharpenMoreFilter::process(const cv::Mat& image) {
    cv::Mat kernel = (cv::Mat_<float>(3,3) << -1, -1, -1, -1, 9, -1, -1, -1, -1);
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
cv::Mat GaussianNoise2Filter::process(const cv::Mat& image) {
    cv::Mat noise(image.size(), image.type());
    cv::randn(noise, 0.0, 10.0);
    
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
cv::Mat SpeckleNoiseFilter::process(const cv::Mat& image) {
    cv::Mat noise(image.size(), CV_32F);
    cv::randn(noise, 0.0, 1.0);
    
    cv::Mat img32;
    image.convertTo(img32, CV_32F);
    
    cv::Mat out = img32 + img32.mul(noise) * 0.1;
    out.convertTo(out, CV_8U);
    return out;
}

// --- threshold_extra_filters.cpp ---
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

cv::Mat EdgePreserveFilter::process(const cv::Mat& image) {
    cv::Mat out;
    if (image.channels() == 3) {
        cv::edgePreservingFilter(image, out, 1, 60.0f, 0.4f);
    } else {
        cv::Mat bgr;
        cv::cvtColor(image, bgr, cv::COLOR_GRAY2BGR);
        cv::Mat res;
        cv::edgePreservingFilter(bgr, res, 1, 60.0f, 0.4f);
        cv::cvtColor(res, out, cv::COLOR_BGR2GRAY);
    }
    return out;
}

cv::Mat StylizationFilter::process(const cv::Mat& image) {
    cv::Mat out;
    if (image.channels() == 3) {
        cv::stylization(image, out, 60.0f, 0.45f);
    } else {
        cv::Mat bgr;
        cv::cvtColor(image, bgr, cv::COLOR_GRAY2BGR);
        cv::Mat res;
        cv::stylization(bgr, res, 60.0f, 0.45f);
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
        { "sigma_r", "Sigma R", SettingType::Number, 0.07, SettingValue(), 0.01, 1.0, 0.01, 1.0, 0.01 }
    };
}

cv::Mat PencilSketchFilter::process(const cv::Mat& image) {
    cv::Mat gray, color;
    float sigmaS = (float)get("sigma_s").toDouble();
    float sigmaR = (float)get("sigma_r").toDouble();
    if (image.channels() == 3) {
        cv::pencilSketch(image, gray, color, sigmaS, sigmaR, 0.05f);
    } else {
        cv::Mat bgr;
        cv::cvtColor(image, bgr, cv::COLOR_GRAY2BGR);
        cv::pencilSketch(bgr, gray, color, sigmaS, sigmaR, 0.05f);
    }
    return gray;
}

cv::Mat EmbossFilter::process(const cv::Mat& image) {
    cv::Mat kernel = (cv::Mat_<float>(3, 3) << -2, -1, 0, -1, 1, 1, 0, 1, 2);
    cv::Mat out;
    cv::filter2D(image, out, -1, kernel);
    return out;
}

cv::Mat QuantizeFilter::process(const cv::Mat& image) {
    cv::Mat img32;
    image.convertTo(img32, CV_32F);
    
    for (int r = 0; r < img32.rows; ++r) {
        float* ptr = img32.ptr<float>(r);
        for (int c = 0; c < img32.cols * img32.channels(); ++c) {
            ptr[c] = std::round(ptr[c] / 32.0f) * 32.0f;
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

