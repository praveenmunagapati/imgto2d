#pragma once
/**
 * edge_blur_filters.h — Edge Detection and Blur Filters
 */
#include "filters/base_filter.h"

// ---------------------------------------------------------------------------
// Edge Filters
// ---------------------------------------------------------------------------
class CannyFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit CannyFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Canny Edge Detection"; }
    QString category() const override { return "Edge Detection"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class SobelFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit SobelFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Sobel Edge Detection"; }
    QString category() const override { return "Edge Detection"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class LaplacianFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit LaplacianFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Laplacian Edge Detection"; }
    QString category() const override { return "Edge Detection"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class PrewittFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit PrewittFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Prewitt Edge"; }
    QString category() const override { return "Edge Detection"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class ScharrFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit ScharrFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Scharr Edge"; }
    QString category() const override { return "Edge Detection"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class DoGFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit DoGFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Difference of Gaussians (DoG)"; }
    QString category() const override { return "Edge Detection"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class RidgeDetectionFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit RidgeDetectionFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Ridge Detection (Hessian)"; }
    QString category() const override { return "Edge Detection"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class HighPassFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit HighPassFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "High Pass"; }
    QString category() const override { return "Edge Detection"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

// ---------------------------------------------------------------------------
// Blur Filters
// ---------------------------------------------------------------------------
class GaussianBlurFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit GaussianBlurFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Gaussian Blur"; }
    QString category() const override { return "Blur/Sharpen"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class MedianBlurFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit MedianBlurFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Median Blur"; }
    QString category() const override { return "Blur/Sharpen"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class BoxBlurFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit BoxBlurFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Box Blur"; }
    QString category() const override { return "Blur/Sharpen"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class MotionBlurFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit MotionBlurFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Motion Blur"; }
    QString category() const override { return "Blur/Sharpen"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class BilateralFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit BilateralFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Bilateral Filter"; }
    QString category() const override { return "Blur/Sharpen"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class LowPassFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit LowPassFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Low Pass"; }
    QString category() const override { return "Blur/Sharpen"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class SharpenMoreFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit SharpenMoreFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Sharpen More"; }
    QString category() const override { return "Blur/Sharpen"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};
