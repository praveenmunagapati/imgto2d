#include "filters/artistic_distort_filters.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>
#include <algorithm>
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

QVector<PFMSetting> OilPaintingFilter::defineSettings() const {
    return {
        { "size", "Size", SettingType::Integer, 5, QVariant(), 1, 20, 1, 20, 1 },
        { "dynRatio", "Dynamic Ratio", SettingType::Integer, 1, QVariant(), 1, 10, 1, 10, 1 }
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

QVector<PFMSetting> DetailEnhanceFilter::defineSettings() const {
    return {
        { "sigma_s", "Sigma S", SettingType::Number, 10.0, QVariant(), 1.0, 200.0, 1.0, 200.0, 1.0 },
        { "sigma_r", "Sigma R", SettingType::Number, 0.15, QVariant(), 0.01, 1.0, 0.01, 1.0, 0.01 }
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

QVector<PFMSetting> PencilSketchFilter::defineSettings() const {
    return {
        { "sigma_s", "Sigma S", SettingType::Number, 60.0, QVariant(), 1.0, 200.0, 1.0, 200.0, 1.0 },
        { "sigma_r", "Sigma R", SettingType::Number, 0.07, QVariant(), 0.01, 1.0, 0.01, 1.0, 0.01 }
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
