#pragma once
#include "filters/base_filter.h"

// ---------------------------------------------------------------------------
// Threshold Filters
// ---------------------------------------------------------------------------
class OtsuThresholdFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit OtsuThresholdFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Otsu Threshold"; }
    QString category() const override { return "Threshold"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class AdaptiveThresholdFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit AdaptiveThresholdFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Adaptive Threshold"; }
    QString category() const override { return "Threshold"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class TruncateThresholdFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit TruncateThresholdFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Truncate Threshold"; }
    QString category() const override { return "Threshold"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class ToZeroThresholdFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit ToZeroThresholdFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "To Zero Threshold"; }
    QString category() const override { return "Threshold"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

// ---------------------------------------------------------------------------
// Extra/Color Filters
// ---------------------------------------------------------------------------
class EqualizeHistFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit EqualizeHistFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Equalize Histogram"; }
    QString category() const override { return "Color"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class AutoContrastFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit AutoContrastFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Auto Contrast"; }
    QString category() const override { return "Color"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class AutoColorFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit AutoColorFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Auto Color"; }
    QString category() const override { return "Color"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class ColorizeFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit ColorizeFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Colorize (Tint)"; }
    QString category() const override { return "Color"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class InvertHueFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit InvertHueFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Invert Hue"; }
    QString category() const override { return "Color"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};
