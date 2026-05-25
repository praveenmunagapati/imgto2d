#include "filters/edge_blur_filters.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

// ---------------------------------------------------------------------------
// Edge Filters
// ---------------------------------------------------------------------------
QVector<PFMSetting> CannyFilter::defineSettings() const {
    return {
        { "threshold1", "Threshold 1", SettingType::Integer, 100, QVariant(), 0, 255, 0, 255, 1 },
        { "threshold2", "Threshold 2", SettingType::Integer, 200, QVariant(), 0, 255, 0, 255, 1 }
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


QVector<PFMSetting> SobelFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 3, QVariant(), 1, 7, 1, 7, 2 },
        { "scale", "Scale", SettingType::Number, 1.0, QVariant(), 0.1, 10.0, 0.1, 10.0, 0.1 }
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


QVector<PFMSetting> LaplacianFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 3, QVariant(), 1, 7, 1, 7, 2 }
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

QVector<PFMSetting> DoGFilter::defineSettings() const {
    return {
        { "sigma1", "Sigma 1", SettingType::Number, 1.0, QVariant(), 0.1, 10.0, 0.1, 10.0, 0.1 },
        { "sigma2", "Sigma 2", SettingType::Number, 2.0, QVariant(), 0.1, 10.0, 0.1, 10.0, 0.1 }
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
QVector<PFMSetting> GaussianBlurFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 5, QVariant(), 1, 51, 1, 51, 2 },
        { "sigma", "Sigma", SettingType::Number, 0.0, QVariant(), 0.0, 10.0, 0.0, 10.0, 0.5 }
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


QVector<PFMSetting> MedianBlurFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 5, QVariant(), 3, 51, 3, 51, 2 }
    };
}

cv::Mat MedianBlurFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    if (ksize % 2 == 0) ksize += 1;

    cv::Mat out;
    cv::medianBlur(image, out, ksize);
    return out;
}


QVector<PFMSetting> BoxBlurFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 5, QVariant(), 1, 51, 1, 51, 1 }
    };
}

cv::Mat BoxBlurFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();

    cv::Mat out;
    cv::blur(image, out, cv::Size(ksize, ksize));
    return out;
}

QVector<PFMSetting> MotionBlurFilter::defineSettings() const {
    return {
        { "size", "Size", SettingType::Integer, 15, QVariant(), 3, 101, 3, 101, 2 }
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

