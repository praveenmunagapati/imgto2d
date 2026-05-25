#pragma once
/**
 * noise_filters.h — Noise and Denoise Filters
 */
#include "filters/base_filter.h"
#include <random>

class GaussianNoiseFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit GaussianNoiseFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Add Gaussian Noise"; }
    QString category() const override { return "Noise"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class SaltAndPepperFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit SaltAndPepperFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Salt & Pepper Noise"; }
    QString category() const override { return "Noise"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class DenoiseFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit DenoiseFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Denoise (NL Means)"; }
    QString category() const override { return "Noise"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class GaussianNoise2Filter : public ImageFilter {
    Q_OBJECT
public:
    explicit GaussianNoise2Filter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Gaussian Noise 2"; }
    QString category() const override { return "Noise"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class SpeckleNoiseFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit SpeckleNoiseFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Speckle Noise"; }
    QString category() const override { return "Noise"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

