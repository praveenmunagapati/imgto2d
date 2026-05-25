#include "filters/noise_filters.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>

// ---------------------------------------------------------------------------
// GaussianNoiseFilter
// ---------------------------------------------------------------------------
QVector<PFMSetting> GaussianNoiseFilter::defineSettings() const {
    return {
        { "mean", "Mean", SettingType::Number, 0.0, QVariant(), -100.0, 100.0, -100.0, 100.0, 1.0 },
        { "sigma", "Std Dev", SettingType::Number, 25.0, QVariant(), 0.0, 100.0, 0.0, 100.0, 1.0 }
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
QVector<PFMSetting> SaltAndPepperFilter::defineSettings() const {
    return {
        { "prob", "Probability", SettingType::Percentage, 5.0, QVariant(), 0.0, 100.0, 0.0, 100.0, 1.0 }
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
QVector<PFMSetting> DenoiseFilter::defineSettings() const {
    return {
        { "h", "Strength (h)", SettingType::Number, 10.0, QVariant(), 1.0, 50.0, 1.0, 50.0, 1.0 }
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
