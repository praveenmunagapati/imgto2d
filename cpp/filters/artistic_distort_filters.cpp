#include "filters/artistic_distort_filters.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>
#include <cmath>

// ---------------------------------------------------------------------------
// Artistic
// ---------------------------------------------------------------------------
QVector<PFMSetting> PosterizeFilter::defineSettings() const {
    return {
        { "levels", "Levels", SettingType::Integer, 4, QVariant(), 2, 256, 2, 256, 1 }
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
QVector<PFMSetting> VignetteFilter::defineSettings() const {
    return {
        { "intensity", "Intensity", SettingType::Number, 150.0, QVariant(), 10.0, 300.0, 10.0, 300.0, 5.0 }
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

QVector<PFMSetting> PixelateFilter::defineSettings() const {
    return {
        { "size", "Pixel Size", SettingType::Integer, 10, QVariant(), 2, 100, 2, 100, 1 }
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

QVector<PFMSetting> WaveFilter::defineSettings() const {
    return {
        { "amplitude", "Amplitude", SettingType::Number, 10.0, QVariant(), 1.0, 100.0, 1.0, 100.0, 1.0 },
        { "frequency", "Frequency", SettingType::Number, 0.05, QVariant(), 0.01, 0.5, 0.01, 0.5, 0.01 }
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
