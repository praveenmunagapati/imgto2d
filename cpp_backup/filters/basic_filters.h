#pragma once
/**
 * basic_filters.h — Basic Image Filters (Brightness, Contrast, Threshold, etc.)
 */
#include "filters/base_filter.h"

class BrightnessFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit BrightnessFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Brightness"; }
    QString category() const override { return "Color"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class ContrastFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit ContrastFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Contrast"; }
    QString category() const override { return "Color"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class InvertFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit InvertFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Invert"; }
    QString category() const override { return "Color"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class ThresholdFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit ThresholdFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Threshold"; }
    QString category() const override { return "Artistic"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class UnsharpMaskFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit UnsharpMaskFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Unsharp Mask"; }
    QString category() const override { return "Blur/Sharpen"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};
