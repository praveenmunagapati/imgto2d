#pragma once
#include "pfm/pfm_base.h"

class _LettersBasePFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit _LettersBasePFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    virtual int getLloydIters() const = 0;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class AdaptiveLettersPFM : public _LettersBasePFM {
    Q_OBJECT
public:
    explicit AdaptiveLettersPFM(QObject* parent = nullptr) : _LettersBasePFM(parent) {}
    QString name() const override { return "Adaptive Letters"; }
protected:
    int getLloydIters() const override { return 0; }
};

class LBGLettersPFM : public _LettersBasePFM {
    Q_OBJECT
public:
    explicit LBGLettersPFM(QObject* parent = nullptr);
    QString name() const override { return "LBG Letters"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    int getLloydIters() const override;
};

class VoronoiLettersPFM : public _LettersBasePFM {
    Q_OBJECT
public:
    explicit VoronoiLettersPFM(QObject* parent = nullptr) : _LettersBasePFM(parent) {}
    QString name() const override { return "Voronoi Letters"; }
protected:
    int getLloydIters() const override;
};
