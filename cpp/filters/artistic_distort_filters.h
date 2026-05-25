#pragma once
#include "filters/base_filter.h"

// ---------------------------------------------------------------------------
// Artistic Filters
// ---------------------------------------------------------------------------
class PosterizeFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit PosterizeFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Posterize"; }
    QString category() const override { return "Artistic"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class EdgePreserveFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit EdgePreserveFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Edge Preserve Smooth"; }
    QString category() const override { return "Artistic"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class StylizationFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit StylizationFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Stylization"; }
    QString category() const override { return "Artistic"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class EmbossFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit EmbossFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Emboss"; }
    QString category() const override { return "Stylize"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class QuantizeFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit QuantizeFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Quantize (8 Colors)"; }
    QString category() const override { return "Artistic"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

// ---------------------------------------------------------------------------
// Distort Filters
// ---------------------------------------------------------------------------
class VignetteFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit VignetteFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Vignette"; }
    QString category() const override { return "Distort"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class PixelateFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit PixelateFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Pixelate"; }
    QString category() const override { return "Distort"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class WaveFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit WaveFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Wave Distortion"; }
    QString category() const override { return "Distort"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};
