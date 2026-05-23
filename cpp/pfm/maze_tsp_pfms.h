#pragma once
#include "pfm/pfm_base.h"

class BaseTSPPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit BaseTSPPFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    virtual std::vector<double> getProbabilities(const cv::Mat& image) = 0;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class TSPClassicPFM : public BaseTSPPFM {
    Q_OBJECT
public:
    explicit TSPClassicPFM(QObject* parent = nullptr) : BaseTSPPFM(parent) {}
    QString name() const override { return "TSP Classic"; }
protected:
    std::vector<double> getProbabilities(const cv::Mat& image) override;
};

class TSPOutlinePFM : public BaseTSPPFM {
    Q_OBJECT
public:
    explicit TSPOutlinePFM(QObject* parent = nullptr) : BaseTSPPFM(parent) {}
    QString name() const override { return "TSP Outline"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    std::vector<double> getProbabilities(const cv::Mat& image) override;
};

class TSPShadingPFM : public BaseTSPPFM {
    Q_OBJECT
public:
    explicit TSPShadingPFM(QObject* parent = nullptr) : BaseTSPPFM(parent) {}
    QString name() const override { return "TSP Shading"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    std::vector<double> getProbabilities(const cv::Mat& image) override;
};

class TSPMSTPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit TSPMSTPFM(QObject* parent = nullptr);
    QString name() const override { return "Minimum Spanning Tree"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
