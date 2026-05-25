#pragma once
/**
 * color_filters.h — Color and Adjustment Filters
 */
#include "filters/base_filter.h"

class GrayscaleFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit GrayscaleFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Grayscale"; }
    QString category() const override { return "Color"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class DesaturateFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit DesaturateFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Desaturate"; }
    QString category() const override { return "Color"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class SaturationFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit SaturationFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Saturation"; }
    QString category() const override { return "Color"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class HueFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit HueFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Hue Shift"; }
    QString category() const override { return "Color"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class GammaFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit GammaFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Gamma Correction"; }
    QString category() const override { return "Color"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class ExposureFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit ExposureFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Exposure"; }
    QString category() const override { return "Color"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class SepiaFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit SepiaFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Sepia"; }
    QString category() const override { return "Color"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class CLAHEFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit CLAHEFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "CLAHE"; }
    QString category() const override { return "Color"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class TemperatureFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit TemperatureFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Temperature"; }
    QString category() const override { return "Color"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};
