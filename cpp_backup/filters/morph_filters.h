#pragma once
#include "filters/base_filter.h"

// ---------------------------------------------------------------------------
// Morphological Filters
// ---------------------------------------------------------------------------
class DilationFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit DilationFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Dilation"; }
    QString category() const override { return "Morphological"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class ErosionFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit ErosionFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Erosion"; }
    QString category() const override { return "Morphological"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class OpeningFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit OpeningFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Opening"; }
    QString category() const override { return "Morphological"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class ClosingFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit ClosingFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Closing"; }
    QString category() const override { return "Morphological"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class MorphGradientFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit MorphGradientFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Morphological Gradient"; }
    QString category() const override { return "Morphological"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class TopHatFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit TopHatFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Top Hat"; }
    QString category() const override { return "Morphological"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class BlackHatFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit BlackHatFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Black Hat"; }
    QString category() const override { return "Morphological"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class DilateCrossFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit DilateCrossFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Dilate Cross"; }
    QString category() const override { return "Morphological"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class ErodeCrossFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit ErodeCrossFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Erode Cross"; }
    QString category() const override { return "Morphological"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class DilateEllipseFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit DilateEllipseFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Dilate Ellipse"; }
    QString category() const override { return "Morphological"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class ErodeEllipseFilter : public ImageFilter {
    Q_OBJECT
public:
    explicit ErodeEllipseFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Erode Ellipse"; }
    QString category() const override { return "Morphological"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};
