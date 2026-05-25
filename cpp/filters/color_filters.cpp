#include "filters/color_filters.h"
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

// ---------------------------------------------------------------------------
// SaturationFilter
// ---------------------------------------------------------------------------
QVector<PFMSetting> SaturationFilter::defineSettings() const {
    return {
        { "saturation", "Saturation", SettingType::Number, 1.0, QVariant(), 0.0, 3.0, 0.0, 3.0, 0.1 }
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
QVector<PFMSetting> HueFilter::defineSettings() const {
    return {
        { "hue", "Hue Shift", SettingType::Integer, 0, QVariant(), -180, 180, -180, 180, 1 }
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
QVector<PFMSetting> GammaFilter::defineSettings() const {
    return {
        { "gamma", "Gamma", SettingType::Number, 1.0, QVariant(), 0.1, 5.0, 0.1, 5.0, 0.1 }
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
QVector<PFMSetting> ExposureFilter::defineSettings() const {
    return {
        { "exposure", "Exposure (EV)", SettingType::Number, 0.0, QVariant(), -5.0, 5.0, -5.0, 5.0, 0.1 }
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
QVector<PFMSetting> CLAHEFilter::defineSettings() const {
    return {
        { "clip_limit", "Clip Limit", SettingType::Number, 2.0, QVariant(), 0.1, 10.0, 0.1, 10.0, 0.1 },
        { "grid_size", "Grid Size", SettingType::Integer, 8, QVariant(), 2, 32, 2, 32, 2 }
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
QVector<PFMSetting> TemperatureFilter::defineSettings() const {
    return {
        { "temperature", "Temp", SettingType::Number, 0.0, QVariant(), -100.0, 100.0, -100.0, 100.0, 1.0 }
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
