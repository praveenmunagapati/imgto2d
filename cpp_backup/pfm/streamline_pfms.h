#pragma once
#include "pfm/pfm_base.h"

class BaseStreamlinePFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit BaseStreamlinePFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};

class StreamlinesEdgeFieldPFM : public BaseStreamlinePFM {
    Q_OBJECT
public:
    explicit StreamlinesEdgeFieldPFM(QObject* parent = nullptr) : BaseStreamlinePFM(parent) {}
    QString name() const override { return "Streamlines Edge Field"; }
protected:
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class StreamlinesFlowFieldPFM : public BaseStreamlinePFM {
    Q_OBJECT
public:
    explicit StreamlinesFlowFieldPFM(QObject* parent = nullptr) : BaseStreamlinePFM(parent) {}
    QString name() const override { return "Streamlines Flow Field"; }
protected:
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class StreamlinesSuperformulaPFM : public BaseStreamlinePFM {
    Q_OBJECT
public:
    explicit StreamlinesSuperformulaPFM(QObject* parent = nullptr) : BaseStreamlinePFM(parent) {}
    QString name() const override { return "Streamlines Superformula"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};
